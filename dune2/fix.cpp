#include "area.h"
#include "bsdata.h"
#include "draw.h"
#include "drawable.h"
#include "fix.h"
#include "game.h"
#include "resid.h"
#include "unit.h"
#include "view.h"

using namespace draw;

static void add_explosion_tile() {
	if(game_chance(30))
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

int fixeffecti::getframe(unsigned& flags, point from, point to) const {
	static int rocket_frame[16] = {0, 1, 2, 3, 4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1};
	static unsigned rocket_flags[16] = {
		0, 0, 0, 0, 0,
		ImageMirrorV, ImageMirrorV, ImageMirrorV, ImageMirrorV,
		ImageMirrorV | ImageMirrorH, ImageMirrorV | ImageMirrorH, ImageMirrorV | ImageMirrorH, ImageMirrorV | ImageMirrorH,
		ImageMirrorH, ImageMirrorH, ImageMirrorH};
	unsigned char d;
	if(from == to)
		return frame;
	switch(count) {
	case 5:
		d = toh(from, to);
		flags |= rocket_flags[d];
		return frame + rocket_frame[d];
	default:
		return frame;
	}
}

BSDATA(fixeffecti) = {
	{"NoEffect"},
	{"ShootHandGun", 80, UNITS1, 23, 1, apply_damage, FixSmallHit},
	{"ShootHeavyGun", 100, UNITS1, 24, 1, apply_damage, FixMediumHit},
	{"ShootBigGun", 100, UNITS1, 25, 1, apply_damage, FixLargeHit},
	{"FireRocket", 100, UNITS, 20, 5, 0, FixExplosion},
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