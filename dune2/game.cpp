#include "area.h"
#include "bsdata.h"
#include "game.h"
#include "player.h"
#include "rand.h"
#include "unit.h"

gamei game;

static void update_standing_units() {
	for(auto& e : bsdata<unit>()) {
		if(!e || e.ismoving())
			continue;
		auto& ei = e.geti();
		if(ei.frame_shoot) {
			if(d100() < 30)
				e.shoot_direction = to(e.shoot_direction, (rand() % 2) ? Left : Right);
		}
	}
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
	case 0: update_standing_units(); break;
	case 1: update_area_decoy(); break;
	}
}