#include "area.h"
#include "answers.h"
#include "bsdata.h"
#include "building.h"
#include "draw.h"
#include "game.h"
#include "math.h"
#include "music.h"
#include "objective.h"
#include "print.h"
#include "pointa.h"
#include "player.h"
#include "rand.h"
#include "textscript.h"
#include "unit.h"
#include "view.h"
#include "view_theme.h"
#include "view_menu.h"
#include "video.h"

gamei game;

unsigned int game_rand() {
	return rand();
}

static void update_area_decoy() {
	for(auto y = 0; y < area.maximum.y; y++)
		for(auto x = 0; x < area.maximum.x; x++)
			area.decoy(point(x, y));
}

static void update_unit_time() {
	for(auto& e : bsdata<unit>()) {
		if(!e)
			continue;
		if(e.shoot_time)
			e.shooting(e.screen, getweapon(e.type), e.get(Attacks));
		while(e.start_time < game.time) {
			auto n = e.start_time;
			e.update();
			if(n == e.start_time && e.start_time <= game.time)
				e.start_time += look_duration; // Pause to think
		}
	}
}

static void add_abilities(unsigned int* v1, const unsigned int* v2) {
	for(auto i = Credits; i <= Supply; i = (statn)(i + 1))
		v1[i] += v2[i];
}

static void update_player_time() {
	// Remove all calculable ability
	for(auto& e : bsdata<playeri>())
		memset(e.objects, 0, sizeof(e.objects));
	// Calculate building count
	for(auto& e : bsdata<building>()) {
		if(e)
			e.getplayer().objects[e.type]++;
	}
	// Calculate units count
	for(auto& e : bsdata<unit>()) {
		if(e)
			e.getplayer().objects[e.type]++;
	}
	// Finally update player values
	for(auto& e : bsdata<playeri>())
		e.update();
}

static void update_building_time() {
	for(auto& e : bsdata<building>()) {
		if(!e)
			continue;
		if(e.shoot_time)
			e.shooting(m2sc(e.position), e.getweapon(), 2);
		e.update();
	}
}

static void update_scouting() {
	for(auto i = 0; i < player_maximum; i++) {
		area.remove(i, Visible);
		// Update visibility on control area (for slabs)
		for(auto y = 0; y < area.maximum.y; y++) {
			for(auto x = 0; x < area.maximum.x; x++) {
				if(area.is(point(x, y), i, Control))
					area.set(point(x, y), i, Visible);
			}
		}
	}
	for(auto& e : bsdata<building>()) {
		if(!e)
			continue;
		e.scouting();
	}
	for(auto& e : bsdata<unit>()) {
		if(!e)
			continue;
		e.scouting();
	}
}

static void get_sand_vibrations(pointa& result) {
	for(auto& e : bsdata<unit>()) {
		if(!e || e.isboard())
			continue;
		auto move = getmove(e.type);
		if(move == Undersand || move == Flying || move == NoMovement)
			continue;
		auto f = area.get(e.position);
		if(f == Sand || f == Dune || f == Spice || f == SpiceRich || f == SpiceBlow)
			result.add(e.position);
	}
}

static void update_worm_sand_sence() {
	pointa vibrations; get_sand_vibrations(vibrations);
	for(auto& e : bsdata<unit>()) {
		if(!e || e.isboard() || e.isorder())
			continue;
		if(e.type == SandWorm)
			e.order = vibrations.nearest(e.position, e.getlos() * 4);
	}
}

static void update_animated_tiles() {
	area.changealternate();
}

static void update_unit_recovery() {
	for(auto& e : bsdata<unit>()) {
		if(e)
			e.recovery();
	}
}

static void update_game_turn() {
	while(game.start_turn < game.time) {
		game.start_turn += look_duration;
		game.turn++;
		update_building_time();
		update_animated_tiles();
		update_player_time();
		update_ai_commands(1 + game.turn % 3);
		switch(game.turn % 6) {
		case 0: update_area_decoy(); break;
		case 1: update_scouting(); break;
		case 2: update_unit_recovery(); break;
		case 3: update_worm_sand_sence(); break;
		}
	}
}

void update_game_time() {
	update_unit_time();
	update_game_turn();
}

static void check_surrounded(point v, terrainn t, terrainn t1) {
	for(auto d : all_directions) {
		auto n = v + getpoint(d);
		if(area.is(n, t) || area.is(n, t1))
			continue;
		area.set(n, t1);
	}
}

static void set_terrain(point v, int value) {
	auto t = area.get(v);
	auto& e = bsdata<terraini>::elements[value];
	if(e.terrain && !e.terrain.is(t))
		return;
	area.set(v, (terrainn)value);
	switch(value) {
	case Mountain: check_surrounded(v, Mountain, Rock); break;
	case SpiceRich: check_surrounded(v, SpiceRich, Spice); break;
	}
}

static void set_terrain_circle(point v, int value, int d) {
	if(d <= 1)
		set_terrain(v, value);
	else {
		rect rc;
		rc.x1 = v.x - d / 2;
		rc.y1 = v.y - d / 2;
		rc.x2 = rc.x1 + d;
		rc.y2 = rc.y1 + d;
		area.set(rc, set_terrain, value);
	}
}

static void set_terrain_small_circle(point v, int value) {
	set_terrain_circle(v, value, xrand(0, 3) - 1);
}

static void set_terrain_circle(point v, int value) {
	set_terrain_circle(v, value, xrand(0, 5) - 2);
}

static void set_terrain_big_circle(point v, int value) {
	set_terrain_circle(v, value, xrand(0, 8) - 2);
}

static void show_introdution() {
	static videoi main_logo[] = {
		{WESTWOOD, 7 * 1000, 110, FG(Disappearing)},
	};
	static videoi main_intro[] = {
		{INTRO1, 9 * 1000, 110, FG(Disappearing)},
		{INTRO2, 8 * 1000, 150, FG(Appearing) | FG(Disappearing), "Intro1"},
		{INTRO3, 7 * 1000, 240, FG(Appearing), "Intro2"},
		{INTRO3, 7 * 1000, 240, FG(ContinueToNext), "Intro3"},
		{INTRO9, 7 * 1000, 230, FG(Appearing), "Intro4"},
		{INTRO9, 7 * 1000, 230, FG(ContinueToNext), "Intro5"},
		{INTRO9, 5 * 1000, 230, FG(ContinueToNext) | FG(Disappearing), "Intro6"},
		{INTRO10, 3 * 1000, 0, FG(Appearing), "Intro7"},
		{INTRO10, 3 * 1000, 0, FG(ContinueToNext), "Intro8"},
		{INTRO11, 3 * 1000, 180, 0, "Intro9"},
		{INTRO11, 3 * 1000, 180, FG(ContinueToNext), "Intro10"},
		{INTRO11, 3 * 1000, 180, 0, "Intro11"},
		{INTRO11, 3 * 1000, 180, FG(ContinueToNext) | FG(Disappearing), "Intro12"},
		{INTRO4, 0, 240, FG(Appearing), "Intro13"},
		{NONE, 6 * 1000, 0, 0, "Intro14"},
		{INTRO6, 10 * 1000, 140, 0, "IntroBattleA"},
		{INTRO7A, 0, 0, 0, "IntroBattleO"},
		{INTRO7B, 0, 0, 0, "IntroBattleO"},
		{INTRO8A, 0, 0, 0, "IntroBattleH"},
		{INTRO8B, 3 * 1000 + 500, 0, FG(Repeated), "IntroBattleH"},
		{INTRO8C, 0, 0, 0, "IntroBattleH"},
		{INTRO5, 8 * 1000, 0, FG(Repeated) | FG(Appearing) | FG(Disappearing), "Intro15"},
		{NONE, 4 * 1000, 0, FG(Appearing), "Intro16"},
		{NONE, 3 * 1000, 0, FG(Disappearing), "Intro17"},
	};
	// Logo theme
	song_play("westwood");
	reset_form_animation();
	reset_video_time();
	video_play(main_logo);
	// Main intro video
	song_play("intro");
	reset_form_animation();
	reset_video_time();
	video_play(main_intro);
	// Change music
	music_play(0);
}

static void load_game() {
	if(confirm("Do you really want to load game?", "Yes", "No"))
		load_game("autosave");
}

static void save_game() {
	save_game("autosave");
}

static void controls_game() {
}

static void restart_scenario() {
}

static void pick_house() {
	if(!confirm(getnm("PickAnotherHouseConfirm"), getnm("Yes"), getnm("No")))
		return;
}

static void quit_game() {
	if(!confirm(getnm("QuitGameConfirm"), getnm("Yes"), getnm("No")))
		return;
}

static point choose_terrain() {
	return show_scene(paint_main_map_choose_terrain, 0, 0);
}

void open_options() {
	static menui elements[] = {
		{"LoadGame", load_game},
		{"SaveGame", save_game},
		{"GameControls", controls_game},
		{"RestartScenario", restart_scenario},
		{"PickAnotherHouse", pick_house},
		{}};
	execute_menu(getnm("GameTitle"), 200, getnm("Cancel"), getnm("QuitGame"), quit_game, elements);
}

void camera_to_base() {
	auto pb = find_base(ConstructionYard, player_index);
	if(pb)
		area.setcamera(pb->position, true);
}

void main_menu() {
	// music_disabled = true;
	show_introdution();
	game.starting_credits = 1000;
	area_generate(SmallMap, 2);
	show_scenario_prompt("Brief", HARVEST, 1);
	camera_to_base();
	show_scene(paint_main_map, 0, 0);
}

static void unit_name(stringbuilder& sb) {
	if(last_unit->getplayer().fraction)
		sb.adds(last_unit->getfractionname());
	sb.adds(last_unit->getname());
}

static void unit_thrall_loading(stringbuilder& sb) {
	sb.add("%1i", last_unit->action * 10);
}

BSDATA(textscript) = {
	{"Unit", unit_name},
	{"UnitThrall", unit_thrall_loading},
};
BSDATAF(textscript)