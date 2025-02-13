#include "area.h"
#include "building.h"
#include "bsdata.h"
#include "draw.h"
#include "fraction.h"
#include "math.h"
#include "resid.h"
#include "rand.h"
#include "pushvalue.h"
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

void human_unit_attack() {
	auto result = choose_terrain();
}

void human_unit_move() {
	auto target = choose_terrain();
	if(!area.isvalid(target))
		return;
	last_unit->move(target);
}

void mouse_unit_move() {
	auto v = (point)draw::hot.param;
	auto p = find_unit(v);
	auto i = 0;
	if(p) {
		// Enemy unit spotted?
	} else {
		auto index = 0;
		for(auto p : human_selected)
			p->move(v, index++);
	}
}

void human_unit_stop() {
}

static void show_introdution() {
	static videoi source[] = {
		{INTRO1, 10 * 1000, 110, FG(Disappearing), "intro"},
		{INTRO2, 8 * 1000, 170, FG(Appearing) | FG(Disappearing), 0, "Intro1"},
		{INTRO3, 0, 0, FG(Appearing), 0, "Intro2"},
		{INTRO3, 0, 0, 0, 0, "Intro3"},
		{INTRO9, 6 * 1000, 220, FG(Appearing), 0, "Intro4"},
		{INTRO9, 6 * 1000, 220, FG(ContinueToNext), 0, "Intro5"},
		{INTRO9, 6 * 1000, 220, FG(ContinueToNext) | FG(Disappearing), 0, "Intro6"},
		{INTRO10, 4 * 1000, 0, FG(Appearing), 0, "Intro7"},
		{INTRO10, 4 * 1000, 0, FG(Appearing)| FG(ContinueToNext), 0, "Intro8"},
		{INTRO11, 6 * 1000, 0, 0, 0, "Intro9"},
		{INTRO11, 6 * 1000, 0, FG(ContinueToNext), 0, "Intro10"},
		{INTRO11, 6 * 1000, 0, FG(ContinueToNext), 0, "Intro11"},
		{INTRO11, 6 * 1000, 0, FG(ContinueToNext), 0, "Intro12"},
		//{INTRO4, 0, 0, FG(Appearing) | FG(Disappearing)},
		//{INTRO6, 0},
		//{INTRO7A, 0},
		//{INTRO7B, 0},
		//{INTRO8A, 0},
		//{INTRO8B, 10000},
		//{INTRO8C, 0},
		//{INTRO5, 4000},
		//{INTRO9, 0},
		//{INTRO10, 4000},
		//{INTRO11, 0, 0, FG(Disappearing)},
	};
	reset_form_animation();
	reset_video_time();
	start_video -= 18 * 1000;
	play_video(source);
}

void main_menu() {
	//show_introdution();
	auto size = sizeof(unit);
	last_fraction = NoFraction;
	animate_id = CONSTRUC;
	area.clear();
	area.random({3, 3, 13, 13}, set_terrain_big_circle, Rock, 12);
	area.random({3, 3, 13, 13}, set_terrain_circle, Mountain, 4);
	area.random({10, 10, 20, 20}, set_terrain_circle, Spice, 20);
	area.random({10, 10, 20, 20}, set_terrain_small_circle, SpiceRich, 5);
	build_structure({5, 5}, Windtrap);
	build_structure({7, 5}, Refinery);
	addobj({5, 7}, Trike, Down);
	addobj({6, 7}, Tank, Down);
	addobj({7, 7}, AssaultTank, Down);
	addobj({8, 7}, Trike, Down);
	show_scene(paint_main_map, 0, 0);
}