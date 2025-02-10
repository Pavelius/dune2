#include "area.h"
#include "game.h"
#include "player.h"

gamei game;

void update_game_turn() {
	game.turn++;
	area.decoy();
}