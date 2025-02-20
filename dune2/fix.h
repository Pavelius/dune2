#pragma once

#include "nameable.h"
#include "point.h"

enum drawprocn : unsigned char;
enum resid : unsigned short;
enum fixn : unsigned char {
	NoEffect,
	ShootAssaultRifle, ShootRotaryCannon, Shoot20mm, Shoot30mm, Shoot155mm, Shoot200mm,
	FireRocket,
	FixSmallHit, FixMediumHit, FixLargeHit,
	FixExplosion, FixBigExplosion,
	FixBikeExplosion, FixBikeExplosionEnd,
	FixHitSand,
};
struct fixeffecti {
	const char*		id;
	short unsigned	milliseconds;
	resid			rid;
	short unsigned	frame, count;
	short unsigned	effect;
	fnevent			apply;
	fixn			next;
	int				getframe(unsigned& flags, point from, point to) const;
};

void add_effect(point from, fixn i);
void add_effect(point from, point to, fixn i);
