#include "area.h"
#include "building.h"
#include "bsdata.h"
#include "fraction.h"
#include "pushvalue.h"
#include "resid.h"
#include "rand.h"
#include "slice.h"
#include "timer.h"
#include "unit.h"
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
	last_unit->order = target;
}

void human_unit_stop() {
}

void main_menu() {
	auto size = sizeof(unit);
	last_fraction = NoFraction;
	mentat_subject = CONSTRUC;
	area.clear();
	area.random({3, 3, 13, 13}, set_terrain_big_circle, Rock, 12);
	area.random({3, 3, 13, 13}, set_terrain_circle, Mountain, 4);
	area.random({10, 10, 20, 20}, set_terrain_circle, Spice, 20);
	area.random({10, 10, 20, 20}, set_terrain_small_circle, SpiceRich, 5);
	build_structure({5, 5}, Windtrap);
	build_structure({7, 5}, Refinery);
	add_unit({5, 7}, Down, Trike, 0);
	add_unit({6, 7}, Down, Trike, 0);
	add_unit({7, 7}, Down, Tank, 0);
	add_unit({8, 7}, Down, AssaultTank, 0);
	show_scene(paint_main_map, 0, 0);
}

