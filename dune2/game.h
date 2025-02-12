#pragma once

struct gamei {
	unsigned long	time;
	unsigned		turn;
};
extern gamei game;

void update_game_time();
void update_game_turn();
