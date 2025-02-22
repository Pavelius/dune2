#include "adat.h"
#include "area.h"
#include "bsdata.h"
#include "building.h"
#include "game.h"
#include "terrain.h"
#include "unit.h"

static int central[4] = {5, 6, 9, 10};
static int nearest[4][2] = {{1, 4}, {2, 7}, {13, 8}, {14, 11}};
static fractionn fractions[player_maximum];

typedef void(*fngenerate)(rect rc);

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

static void rock_region(rect rc) {
	area.random(rc, set_terrain_big_circle, Rock, 12);
	area.random(rc, set_terrain_circle, Mountain, 4);
}

static void dune_region(rect rc) {
}

static void spice_region(rect rc) {
	area.random(rc, set_terrain_circle, Spice, 20);
	area.random(rc, set_terrain_small_circle, SpiceRich, 5);
}

static void create_player() {
	auto player = bsdata<playeri>::add();
	player->clear();
	player_index = player->getindex();
	player->add(Credits, game.starting_credits);
	player->fraction = fractions[player_index];
	player->color_index = player->getfraction().default_color;
}

static void player_base(rect rc) {
	create_player();
	auto v = center(rc);
	area.set({v.x - 2, v.y - 2, v.x + 4, v.y + 4}, set_terrain, Rock);
	rock_region(rc);
	add_building(v, ConstructionYard);
	add_unit(v, Trike, Down);
	add_unit(v, Harvester, Down);
	add_unit(v, LightInfantry, Down);
	add_unit(v, LightInfantry, Down);
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
		add_region(regions, indecies[i], player_base);
		add_region(regions, central[i], spice_region);
		if(game_chance(50)) {
			add_region(regions, nearest[i][0], spice_region);
			add_region(regions, nearest[i][1], rock_region);
		} else {
			add_region(regions, nearest[i][1], spice_region);
			add_region(regions, nearest[i][0], rock_region);
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
	add_players(regions, number_of_players);
	add_region(regions, dune_region);
	generate_lands(regions);
}