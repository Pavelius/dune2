#pragma once

struct sprite;

enum objectn : unsigned char;

enum resid : unsigned char {
	NONE,
	FONT6, FONT8, FONT10, FONT3, FONT16,
	MENTATS, MENSHPA, MENSHPH, MENSHPO, MENSHPM,
	SHAPES, ICONS, ARROWS, MOUSE, SCREEN, CHOAM, DUNEMAP, PIECES,
	UNITS, UNITS1, UNITS2,
	SLAB4, AFINALA, AFINALB, BARRAC, CARRYALL,
	CONSTRUC, EFINALA, EFINALB, FARTR, FHARK,
	FORDOS, FREMEN, GOLDBB, HARKTANK, HARVEST,
	HEADQRTS, HFINALA, HFINALB, HFINALC, HITCFTRY,
	HTANK, HVYFTRY, HYINFY, INFANTRY, INTRO1,
	INTRO10, INTRO11, INTRO2, INTRO3, INTRO4,
	INTRO5, INTRO6, INTRO7A, INTRO7B, INTRO8A,
	INTRO8B, INTRO8C, INTRO9, IX, LITEFTRY,
	LOSTBILD, LOSTVEHC, LTANK, MCV, MEANWHIL,
	OFINALA, OFINALB, OFINALC, OFINALD, ORDRTANK,
	ORNI, OTRIKE, PALACE, QUAD, REFINERY,
	REPAIR, RTANK, RTURRET, SABOTURE, SARDUKAR,
	SLAB, STANK, STARPORT, STATIC, STORAGE,
	TRIKE, TURRET, WALL, WESTWOOD, WIN1,
	WIN2, WINDTRAP, WOR, WORM,
	LastRes = WORM
};
extern sprite* res_data[LastRes + 1];

enum musicn : unsigned char {
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
extern musicn current_music;

enum messagen : unsigned char {
	ProgramTitle, Cancel, Yes, No,
	ChooseTarget, ChoosePlacement, ChooseSubject, SelectHouse,
	StartGame, LoadGame, SaveGame, HallOfFame, QuitGame, ResumeGame, Exit, Proceed, Repeat, BuildIt,
	GameControls, RestartScenario, PickAnotherHouse, Cost,
	CompleteProgress, UnitDestroyed, UnitDevoured, ReachSpiceMaximum,
	HarvesterWork, UnitCantDoThis, HarvesterUnload, HarvesterReturn,
	ChooseTargetInfo, ChoosePlacementInfo,
	ConfirmJoinHouse, ConfirmLoadGame, ConfirmPickAnotherHouse, ConfirmQuitGame,
	CantSupportEnergy,
	LastMessage = CantSupportEnergy,
};
extern const char* message_names[LastMessage + 1];

enum scenariotextn : unsigned char {
	ScenarioBrief, ScenarioWin, ScenarioLose, ScenarioAdvice,
	LastScenatioText = ScenarioAdvice
};

extern unsigned long current_tick;

extern const char* intro_subtitle[];
extern const char* scenario_a_text[40];
extern const char* scenario_h_text[40];
extern const char* scenario_o_text[40];
extern const char* regions_conquere[];

struct gamei {
	unsigned long	time;
	unsigned long	start_turn;
	unsigned		turn;
	unsigned		starting_credits;
	unsigned char	scenario; // 1...9
	objectn			house;
	objectn			controls[27];
};
extern gamei game;

void camera_to_base();
bool confirm_join_house(objectn subject);
void game_file(char* result, const char* folder, const char* name, const char* ext, int index = 0);
bool is_combat(musicn v);
bool is_map_music(musicn v);
void load_game(const char* id);
void open_options();
void save_game(const char* id);
void set_region_control(objectn fraction, unsigned index);
void set_region_header(const char* format);
void show_scenario_moves(objectn fraction, unsigned char scenario);
void show_scenario_prompt(objectn fraction, unsigned char level, scenariotextn type);
void update_ai_commands(unsigned char player);
void update_game_time();
void choose_game_house();

inline const sprite* gres(resid i) { return res_data[i]; }
inline const char* getnm(messagen v) { return message_names[v]; }