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
		e.abilities[SpiceCapacity] = 0;
	}
	// Calculate new values
	for(auto& e : bsdata<building>()) {
		switch(e.type) {
		case Refinery: case SpiceSilo: bsdata<playeri>::elements[e.player].add(SpiceCapacity, 1000); break;
		case Windtrap: bsdata<playeri>::elements[e.player].add(Energy, 100); break;
		}
	}
}

static void update_building_time() {
	for(auto& e : bsdata<building>()) {
		if(!e)
			continue;
		e.update();
	}
}

static void update_game_turn() {
	while(game.start_turn < game.time) {
		game.start_turn += 500;
		game.turn++;
		update_building_time();
		// Some visual effect
		switch(game.turn % 10) {
		case 0: update_area_decoy(); break;
		}
	}
}

void update_game_time() {
	update_unit_time();
	update_game_turn();
}