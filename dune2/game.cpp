#include "area.h"
#include "bsdata.h"
#include "building.h"
#include "game.h"
#include "player.h"
#include "rand.h"
#include "unit.h"

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
		if(!e.start_time)
			e.start_time = game.time;
		while(e.start_time < game.time) {
			auto n = e.start_time;
			e.update();
			if(n == e.start_time)
				e.start_time += 1000; // Pause to think
		}
	}
}

static void update_player_time() {
	// Remove all calculable ability
	for(auto& e : bsdata<playeri>()) {
		e.abilities[Energy] = 0;
		e.abilities[EnergyCapacity] = 0;
		e.abilities[SupplyCapacity] = 0;
		e.abilities[SpiceCapacity] = 0;
		memset(e.buildings, 0, sizeof(e.buildings));
		memset(e.units, 0, sizeof(e.units));
	}
	// Calculate building count
	for(auto& e : bsdata<building>()) {
		if(!e)
			continue;
		e.getplayer().buildings[e.type]++;
	}
	// Calculate units count
	for(auto& e : bsdata<unit>()) {
		if(!e)
			continue;
		e.getplayer().units[e.type]++;
	}
}

static void update_building_time() {
	for(auto& e : bsdata<building>()) {
		if(!e)
			continue;
		e.update();
	}
}

static void update_scouting() {
	for(auto i = 0; i < playeri::maximum; i++)
		area.remove(i, Visible);
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

static void update_game_turn() {
	while(game.start_turn < game.time) {
		game.start_turn += 500;
		game.turn++;
		update_building_time();		
		// Some visual effect
		switch(game.turn % 6) {
		case 0: update_area_decoy(); break;
		case 1: update_player_time(); break;
		case 2: update_scouting(); break;
		}
	}
}

void update_game_time() {
	update_unit_time();
	update_game_turn();
}