#pragma once

enum resid : unsigned short;
enum objectn : unsigned char;

enum soundn : unsigned char {
	NoMusic,
	MusicAblient01, MusicAblient02, MusicAblient03, MusicAblient04, MusicAblient05,
	MusicAblient06, MusicAblient07, MusicAblient08, MusicAblient09, MusicAblient10,
	MusicAttack01, MusicAttack02, MusicAttack03, MusicAttack04, MusicAttack05, MusicAttack06,
	MusicWestwood, MusicIntro, MusicTitle, MusicConquest, MusicCredites, MusicEmperor,
	MusicMentatA, MusicMentatH, MusicMentatO,
	MusicDefeatA, MusicDefeatH, MusicDefeatO,
	MusicEndingA, MusicEndingH, MusicEndingO,
	MusicVictoryA, MusicVictoryH, MusicVictoryO,
};
extern soundn current_music;

enum messagen : unsigned char {
	ProgramTitle, Cancel, Yes, No,
	ChooseTarget, ChoosePlacement, ChooseSubject, SelectHouse,
	LoadGame, SaveGame, QuitGame, ResumeGame, Exit, Proceed, Repeat, BuildIt,
	GameControls, RestartScenario, PickAnotherHouse, Cost,
	CompleteProgress,
	ChooseTargetInfo, ChoosePlacementInfo,
	ConfirmLoadGame, ConfirmPickAnotherHouse, ConfirmQuitGame,
	CantSupportEnergy,
	LastMessage = CantSupportEnergy,
};
extern const char* message_names[LastMessage + 1];

struct gamei {
	unsigned long	time;
	unsigned long	start_turn;
	unsigned		turn;
	unsigned		starting_credits;
	int				scenario;
	objectn			house;
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
void update_ai_commands(unsigned char player);
void update_game_time();
void choose_game_house();

inline const char* getnm(messagen v) { return message_names[v]; }