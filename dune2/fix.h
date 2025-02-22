#pragma once

#include "flagable.h"
#include "nameable.h"
#include "point.h"

enum drawprocn : unsigned char;
enum resid : unsigned short;

enum fixn : unsigned char {
	NoEffect,
	Smoke, BurningFire,
	ShootAssaultRifle, ShootRotaryCannon, Shoot20mm, Shoot30mm, Shoot155mm, Shoot200mm,
	FireRocket,
	FixSmallHit, FixMediumHit, FixLargeHit,
	FixExplosion, FixBigExplosion,
	FixBikeExplosion, FixBikeExplosionEnd,
	FixHitSand,
};

typedef void(*fnevent)();
typedef fixn (*fnnext)();

struct fixeffecti {
	const char*		id;
	short unsigned	milliseconds;
	resid			rid;
	short unsigned	frame, count;
	short unsigned	effect;
	fnevent			apply;
	fixn			next;
	fnnext			next_proc;
	int				getframe(unsigned& flags, point from, point to) const;
};
struct fixable {
	flag16			fixeffects;
	bool			is(fixn i) const { return fixeffects.is(i); }
	void			set(fixn i) { fixeffects.set(i); }
	void			remove(fixn i) { fixeffects.remove(i); }
};

void add_effect(point from, fixn i);
void add_effect(point from, point to, fixn i);
