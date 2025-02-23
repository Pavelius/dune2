#pragma once

enum resid : unsigned short;
struct gamei {
	unsigned long	time;
	unsigned long	start_turn;
	unsigned		turn;
	unsigned		starting_credits;
};
extern gamei game;

unsigned int game_rand();
inline bool game_chance(unsigned int value) { return (game_rand() % 100) < value; }
inline unsigned int game_rand(int from, int to) { return from + game_rand() % (to - from + 1); }

void camera_to_base();
void load_game(const char* id);
void open_options();
void save_game(const char* id);
void show_scenario_prompt(const char* id, resid rid, int level);
void update_game_time();
