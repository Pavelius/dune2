#pragma once

#include "nameable.h"

enum drawprocn : unsigned char;
enum resid : unsigned short;
enum fixn : unsigned char {
	NoEffect,
	FixExplosion, FixBigExplosion,
	FixBikeExplosion, FixBikeExplosionEnd,
	FixHitSand,
};
struct fixeffecti {
	const char*		id;
	short unsigned	milliseconds;
	resid			rid;
	short unsigned	frame, count;
	fnevent			apply;
	fixn			next;
};

void add_effect(point s, fixn i);
inline void add_area_effect(point v, fixn i) { add_effect(m2sc(v), i); }
