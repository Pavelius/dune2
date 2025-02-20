#pragma once

#include "nameable.h"
#include "point.h"

enum drawprocn : unsigned char;
enum resid : unsigned short;
enum fixn : unsigned char {
	NoEffect,
	ShootHandGun, ShootHeavyGun, ShootBigGun,
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
	fnevent			apply;
	fixn			next;
	int				getframe(unsigned& flags, point from, point to) const;
};

void add_effect(point from, fixn i, short unsigned owner);
void add_effect(point from, point to, fixn i, short unsigned owner);
