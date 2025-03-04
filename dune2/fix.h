#pragma once

#include "flagable.h"
#include "nameable.h"
#include "point.h"

enum drawprocn : unsigned char;
enum resid : unsigned short;

enum fixn : unsigned char {
	NoEffect,
	Smoke, BurningFire, BurningFireEnd,
	ShootAssaultRifle, ShootRotaryCannon, Shoot20mm, Shoot30mm, Shoot155mm, Shoot200mm,
	HandRocket, FireRocket,
	FixSmallHit, FixMediumHit, FixLargeHit,
	FixExplosion, FixBigExplosion,
	FixBikeExplosion, FixBikeExplosionEnd,
	WormDevour, FixHitSand,
};

typedef void(*fnevent)();
typedef fixn (*fnnext)();

struct fixeffecti {
	const char*		id;
	short unsigned	milliseconds;
	resid			rid;
	short unsigned	frame, count;
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
