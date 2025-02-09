#include "area.h"
#include "bsdata.h"
#include "fraction.h"
#include "pushvalue.h"
#include "resid.h"
#include "rand.h"
#include "slice.h"
#include "timer.h"
#include "view.h"

static void check_surrounded(pointc v, terrainn t, terrainn t1) {
	for(auto d : all_strait_directions) {
		auto n = v + getpoint(d);
		if(area.isn(n, t))
			continue;
		area.set(n, t1);
	}
}

static void set_terrain(pointc v, int value) {
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

static void set_terrain_circle(pointc v, int value, int d) {
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

static void set_terrain_small_circle(pointc v, int value) {
	set_terrain_circle(v, value, xrand(0, 3) - 1);
}

static void set_terrain_circle(pointc v, int value) {
	set_terrain_circle(v, value, xrand(0, 5) - 2);
}

static void set_terrain_big_circle(pointc v, int value) {
	set_terrain_circle(v, value, xrand(0, 8) - 2);
}

void update_decoy() {
}

void main_menu() {
	last_fraction = NoFraction;
	mentat_subject = CONSTRUC;
	area.clear();
//	area.set({2, 4}, Rock);
//	area.set({3, 4}, Rock);
//	area.set({3, 4, 5, 6}, set_terrain, Rock);
	area.random({3, 3, 13, 13}, set_terrain_big_circle, Rock, 12);
	area.random({3, 3, 13, 13}, set_terrain_circle, Mountain, 4);
	area.random({10, 10, 20, 20}, set_terrain_circle, Spice, 20);
	area.random({10, 10, 20, 20}, set_terrain_small_circle, SpiceRich, 5);
	// FONT3 - Monochrome.
	// FONT6, FONT8  - Three colors: 1 - main, 2 - right shadow, 3 - left shadow
	// FONT10 - Capital in other color. 15 - shadow, (12, 10, 8) Capital marked
	// FONT16 - Two colors: 1 - main, 5 - second main, 6 - part shadow
	show_scene(paint_main_map, 0, 0);
}

