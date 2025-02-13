#pragma once

struct gamei {
	unsigned long	time;
	unsigned		turn;
};
extern gamei game;

unsigned int game_rand();
inline bool game_chance(unsigned int value) { return (game_rand() % 100) < value; }
inline unsigned int game_rand(int from, int to) { return from + game_rand() % (to - from + 1); }

void update_game_time();
void update_game_turn();
