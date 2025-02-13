#include "area.h"
#include "bsdata.h"
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

void update_game_turn() {
	game.turn++;
	// Some visual effect
	switch(game.turn % 5) {
	case 0: update_area_decoy(); break;
	}
}

static void update_unit_time() {
	auto push = last_unit;
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
	last_unit = push;
}

void update_game_time() {
	update_unit_time();
}