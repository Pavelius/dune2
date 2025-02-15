#include "area.h"
#include "building.h"
#include "bsdata.h"
#include "draw.h"
#include "fraction.h"
#include "math.h"
#include "music.h"
#include "order.h"
#include "pushvalue.h"
#include "resid.h"
#include "rand.h"
#include "slice.h"
#include "timer.h"
#include "unit.h"
#include "unita.h"
#include "video.h"
#include "view.h"

static void check_surrounded(point v, terrainn t, terrainn t1) {
	for(auto d : all_strait_directions) {
		auto n = v + getpoint(d);
		if(area.isn(n, t))
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

static void build_structure(point v, buildingn type) {
	auto& e = bsdata<buildingi>::elements[type];
	area.set(v, e.shape, e.frames);
}

static point choose_terrain() {
	return show_scene(paint_main_map_choose_terrain, 0, 0);
}

static void apply_units_order(point v) {
	human_selected.order(Move, Center, v, false);
}

void human_unit_attack() {
	auto target = choose_terrain();
	if(!area.isvalid(target))
		return;
}

void human_unit_move() {
	auto target = choose_terrain();
	if(!area.isvalid(target))
		return;
	apply_units_order(target);
}

void mouse_unit_move() {
	auto v = (point)draw::hot.param;
	auto p = find_unit(v);
	auto i = 0;
	if(p) {
		// Enemy unit spotted?
	} else
		apply_units_order(v);
}

void human_unit_stop() {
}

static void show_introdution() {
	static videoi source[] = {
		// {WESTWOOD, 8 * 1000, 110, FG(Appearing) | FG(Disappearing), "westwood"},
		{INTRO1, 9 * 1000, 110, FG(Disappearing)}, // 00:18
		{INTRO2, 8 * 1000, 150, FG(Appearing) | FG(Disappearing), "Intro1"}, // 00:28
		{INTRO3, 7 * 1000, 240, FG(Appearing), "Intro2"}, // 00:38
		{INTRO3, 7 * 1000, 240, FG(ContinueToNext), "Intro3"}, // 00:46
		{INTRO9, 7 * 1000, 230, FG(Appearing), "Intro4"}, // 00:53
		{INTRO9, 7 * 1000, 230, FG(ContinueToNext), "Intro5"}, // 01:01
		{INTRO9, 5 * 1000, 230, FG(ContinueToNext) | FG(Disappearing), "Intro6"}, // 01:09
		{INTRO10, 3 * 1000, 0, FG(Appearing), "Intro7"},  // 01:15
		{INTRO10, 3 * 1000, 0, FG(ContinueToNext), "Intro8"}, // 01:19
		{INTRO11, 3 * 1000, 180, 0, "Intro9"}, // 01:22
		{INTRO11, 3 * 1000, 180, FG(ContinueToNext), "Intro10"}, // 01:25
		{INTRO11, 3 * 1000, 180, 0, "Intro11"}, // 01:28
		{INTRO11, 3 * 1000, 180, FG(ContinueToNext) | FG(Disappearing), "Intro12"}, // 01:31
		{INTRO4, 0, 240, FG(Appearing), "Intro13"},
		{NONE, 6 * 1000, 0, 0, "Intro14"},
		{INTRO6, 10 * 1000, 140, 0, "IntroBattleA"},
		{INTRO7A, 0, 0, 0, "IntroBattleO"},
		{INTRO7B, 0, 0, 0, "IntroBattleO"},
		{INTRO8A, 0, 0, 0, "IntroBattleH"},
		{INTRO8B, 3 * 1000 + 500, 0, FG(Repeated), "IntroBattleH"},
		{INTRO8C, 0, 0, FG(Disappearing), "IntroBattleH", color(255, 255, 255)},
		{INTRO5, 7 * 1000, 0, FG(Repeated) | FG(Appearing) | FG(Disappearing), "Intro15"},
		{NONE, 4 * 1000, 0, FG(Appearing), "Intro16"},
		{NONE, 3 * 1000, 0, FG(Disappearing), "Intro17"},
	};
	reset_form_animation();
	reset_video_time();
	start_video -= 18 * 1000;
	video_dump("test/intro.txt", source, 18 * 1000);
	song_play("intro");
	video_play(source);
	music_play(0);
}

void main_menu() {
	show_introdution();
	auto size = sizeof(unit);
	last_fraction = NoFraction;
	player = bsdata<playeri>::elements;
	player->add(Credits, 1000);
	player->color_index = 2;
	area.clear();
	area.random({3, 3, 13, 13}, set_terrain_big_circle, Rock, 12);
	area.random({3, 3, 13, 13}, set_terrain_circle, Mountain, 4);
	area.random({10, 10, 20, 20}, set_terrain_circle, Spice, 20);
	area.random({10, 10, 20, 20}, set_terrain_small_circle, SpiceRich, 5);
	add_building({5, 5}, Windtrap);
	add_building({7, 5}, Refinery);
	add_unit({5, 7}, Trike, Down);
	add_unit({6, 7}, Tank, Down);
	add_unit({7, 7}, AssaultTank, Down);
	add_unit({8, 7}, Trike, Down);
	show_scene(paint_main_map, 0, 0);
}