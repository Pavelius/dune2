#pragma once

struct sprite;

enum resid : unsigned short {
	NONE,
	FONT6, FONT8, FONT10, FONT3, FONT16,
	MENTATS, MENSHPA, MENSHPH, MENSHPO, MENSHPM,
	SHAPES, ICONS, ARROWS, MOUSE, SCREEN, CHOAM,
	FARTR, FHARK, FORDOS,
	HYINFY, INFANTRY, TRIKE, LTANK, HTANK,
	SLAB4, BARRAC, CARRYALL, CONSTRUC, HEADQRTS, REFINERY, STORAGE, WALL, WINDTRAP,
	WESTWOOD, INTRO1, INTRO2, INTRO3, INTRO4, INTRO5,
	INTRO6, INTRO7A, INTRO7B, INTRO8A, INTRO8B, INTRO8C, INTRO9, INTRO10, INTRO11,
	UNITS, UNITS1, UNITS2,
};
struct residi {
	const char*	id;
	const char*	folder;
	sprite*		data;
	int			size;
	bool		error;
	sprite*		get();
};
void clear_sprites();
void save_sprite(resid id);
const sprite* gres(resid i);
// const sprite* gres(const char* id);