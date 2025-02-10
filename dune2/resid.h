#pragma once

struct sprite;

enum resid : unsigned short {
	NONE,
	FONT6, FONT8, FONT10, FONT3, FONT16,
	MENTATS, MENSHPA, MENSHPH, MENSHPO, MENSHPM,
	SHAPES, ICONS, ARROWS, SCREEN,
	SLAB4, BARRAC, CARRYALL, CONSTRUC,
	UNITS, UNITS1, UNITS2,
};
struct residi {
	const char*	id;
	const char*	folder;
	sprite*		data;
	bool		error;
	sprite*		get();
};
void clear_sprites();
sprite* gres(resid i);
sprite* gres(const char* id);