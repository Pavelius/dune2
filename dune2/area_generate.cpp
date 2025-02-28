#include "adat.h"
#include "area.h"
#include "bsdata.h"
#include "building.h"
#include "game.h"
#include "objective.h"
#include "terrain.h"
#include "unit.h"

static int central[4] = {5, 6, 9, 10};
static int nearest[4][2] = {{1, 4}, {2, 7}, {13, 8}, {14, 11}};
static objectn fractions[player_maximum];

typedef void(*fngenerate)(rect rc);

static void check_surrounded(point v, terrainn t, terrainn t1) {
	for(auto d : all_directions) {
		auto n = v + getpoint(d);
		if(area.is(n, t) || area.is(n, t1))
			continue;
		area.set(n, t1);
	}
}

static void check_surrounded(terrainn t, terrainn t1) {
	for(auto y = 0; y < area.maximum.y; y++) {
		for(auto x = 0; x < area.maximum.x; x++) {
			auto v = point(x, y);
			auto n = area.get(v);
			if(n == t)
				check_surrounded(v, t, t1);
		}
	}
}

static void set_terrain(point v, int value) {
	auto t = area.get(v);
	auto& e = bsdata<terraini>::elements[value];
	if(e.terrain && !e.terrain.is(t))
		return;
	area.set(v, (terrainn)value);
}

static void set_terrain_circle(point v, int value, int d) {
	if(d <= 1)
		set_terrain(v, value);
	else {
		rect rc;
		rc.x1 = v.x - d / 2;
		rc.y1 = v.y - d / 2;
		rc.x2 = rc.x1 + d + 1;
		rc.y2 = rc.y1 + d + 1;
		area.set(rc, set_terrain, value);
	}
}

static void set_terrain_small_circle(point v, int value) {
	set_terrain_circle(v, value, xrand(0, 3) - 1);
}

static void set_terrain_circle(point v, int value) {
	set_terrain_circle(v, value, xrand(1, 5) - 2);
}

static void set_terrain_big_circle(point v, int value) {
	set_terrain_circle(v, value, xrand(2, 6));
}

static void add_random(const rect& rc, areai::fnset proc, int value, int count) {
	auto v = center(rc);
	area.random(v, rc.width() / 4, rc.width() / 4, proc, value, count);
}

static void rock_region(rect rc) {
	set_terrain_big_circle(center(rc), Rock);
	add_random(rc, set_terrain_big_circle, Rock, 5);
	add_random(rc, set_terrain_circle, Mountain, 3);
}

static void dune_region(rect rc) {
	set_terrain_circle(center(rc), Dune);
	add_random(rc, set_terrain_circle, Dune, 5);
	auto push_player = player_index;
	player_index = 0;
	// add_unit(center(rc), SandWorm, Down);
	player_index = push_player;
}

static void spice_region(rect rc) {
	set_terrain_circle(center(rc), Spice);
	area.random(rc, set_terrain_circle, Spice, 7);
	area.random(rc, set_terrain_small_circle, SpiceRich, 5);
}

static void create_player() {
	auto player = bsdata<playeri>::add();
	player->clear();
	player_index = player->getindex();
	player->add(Credits, game.starting_credits);
	player->fraction = fractions[player_index];
	player->color_index = getdefaultcolor(player->fraction);
}

static void set_player_region(point v) {
	auto player = bsdata<playeri>::elements + player_index;
	player->region = r2i(area.m2r(v));
}

static void add_neutral_player() {
	auto player = bsdata<playeri>::add();
	player->clear();
	player_index = player->getindex();
}

static void add_unit(point v, objectn u) {
	area.blockland(Tracked, 0);
	blockunits();
	auto v1 = area.nearest(v, isfreetrack, 5);
	add_unit(v1, u, Down, player_index);
}

static int center_expanse(int r) {
	switch(r) {
	case 15: return 10;
	case 12: return 9;
	case 0: return 5;
	case 3: return 6;
	default: return -1;
	}
}

static void player_base(rect rc) {
	create_player();
	auto v = center(rc);
	area.set({v.x - 3, v.y - 3, v.x + 6, v.y + 6}, set_terrain, Rock);
	add_random(rc, set_terrain_big_circle, Rock, 6);
	add_building(v, ConstructionYard);
	add_unit(v, Harvester);
	//add_unit(v, LightInfantry);
	//add_unit(v, HeavyInfantry);
	add_unit(v, Trike);
	//add_unit(v, Quad);
	//add_unit(v, AssaultTank);
	add_unit(v, RocketTank);
	auto region = r2i(area.m2r(v));
	auto p1 = add_objective(ExploreArea, player_index, region);
	auto p2 = add_objective(ExploreArea, player_index, center_expanse(region));
	// p2->setparent(p1);
}

static void add_region(fngenerate* regions, int index, fngenerate value) {
	if(index >= area.region_maximum)
		return;
	regions[index] = value;
}

static size_t random_empty_index(fngenerate* regions) {
	adat<int, 16> indecies;
	for(size_t i = 0; i < area.region_maximum; i++) {
		if(!regions[i])
			indecies.add(i);
	}
	if(!indecies)
		return -1;
	return indecies.random();
}

static void add_region(fngenerate* regions, fngenerate value) {
	auto index = random_empty_index(regions);
	if(index == -1)
		return;
	regions[index] = value;
}

static void add_players(fngenerate* regions, size_t count) {
	int indecies[4] = {0, 3, 12, 15};
	zshuffle(indecies, sizeof(indecies) / sizeof(indecies[0]));
	for(size_t i = 0; i < count; i++) {
		auto ri = indecies[i];
		add_region(regions, ri, player_base);
		add_region(regions, region_central[ri], spice_region);
		if(game_chance(50)) {
			add_region(regions, region_near_h[ri], spice_region);
			add_region(regions, region_near_v[ri], rock_region);
		} else {
			add_region(regions, region_near_v[ri], spice_region);
			add_region(regions, region_near_h[ri], rock_region);
		}
	}
}

static void generate_lands(fngenerate* regions) {
	for(size_t i = 0; i < area.region_maximum; i++) {
		if(regions[i])
			regions[i](area.regions[i]);
	}
}

static void initialize_random_fractions() {
	fractions[0] = Atreides;
	fractions[1] = Harkonens;
	fractions[2] = Ordos;
	fractions[3] = Atreides;
	fractions[4] = Harkonens;
	fractions[5] = Ordos;
	zshuffle(fractions, sizeof(fractions) / sizeof(fractions[0]));
}

void area_generate(areasizen n, int number_of_players) {
	fngenerate regions[area.region_maximum] = {};
	initialize_random_fractions();
	bsdata<playeri>::source.clear();
	area.clear(SmallMap);
	add_neutral_player();
	add_players(regions, number_of_players);
	add_region(regions, dune_region);
	generate_lands(regions);
	check_surrounded(SpiceRich, Spice);
	check_surrounded(Mountain, Rock);
}