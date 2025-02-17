#include "area.h"
#include "bsdata.h"
#include "drawable.h"
#include "fix.h"
#include "resid.h"
#include "unit.h"
#include "view.h"

static void add_explosion_tile() {
	area.set(s2m(last_object->screen), Explosion);
}

static void apply_damage() {
	auto p = (draweffect*)last_object;
	auto v = s2m(last_object->screen);
	if(p->owner == 0xFFFF)
		return;
	auto pa = bsdata<unit>::elements + p->owner;
	auto pe = find_unit(v);
	if(!pe)
		return;
	auto damage = pa->get(Damage) - pe->get(Armor);
	if(damage <= 0)
		damage = 1;
	pe->damage(damage);
}

void add_effect(point v, fixn i, short unsigned owner) {
	add_effect(v, i, animate_time, owner);
}

void add_effect(point from, point to, fixn i, short unsigned owner) {
	add_effect(from, to, i, animate_time, owner);
}

BSDATA(fixeffecti) = {
	{"NoEffect"},
	{"ShootHandGun", 80, UNITS1, 23, 1, apply_damage, FixSmallHit},
	{"ShootHeavyGun", 100, UNITS1, 24, 1, apply_damage, FixMediumHit},
	{"ShootBigGun", 100, UNITS1, 25, 1, apply_damage, FixLargeHit},
	{"FixSmallHit", 0, UNITS1, 2, 1, add_explosion_tile},
	{"FixMediumHit", 0, UNITS1, 3, 1, add_explosion_tile},
	{"FixLargeHit", 0, UNITS1, 4, 1, add_explosion_tile},
	{"FixExplosion", 0, UNITS1, 32, 5, add_explosion_tile},
	{"FixBigExplosion", 0, UNITS1, 37, 5, add_explosion_tile},
	{"FixBikeExplosion", 100, UNITS1, 32, 2, 0, FixBikeExplosionEnd},
	{"FixBikeExplosionEnd", 100, UNITS1, 0, 2, add_explosion_tile},
	{"FixHitSand", 200, UNITS1, 5, 3, add_explosion_tile},
};
assert_enum(fixeffecti, FixHitSand)