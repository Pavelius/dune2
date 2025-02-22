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
	auto pe = find_unit(v);
	if(!pe)
		return;
	auto damage = p->effect - pe->get(Armor);
	if(damage <= 0) {
		if(game_chance(50))
			damage = 1;
	}
	if(damage <= 0)
		return;
	pe->damage(damage);
}

void add_effect(point v, fixn i) {
	add_effect(v, i, animate_time, bsdata<fixeffecti>::elements[i].effect);
}

void add_effect(point from, point to, fixn i) {
	add_effect(from, to, i, animate_time, bsdata<fixeffecti>::elements[i].effect);
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
	{"Smoke", 100, UNITS1, 29, 3},
	{"BurningFire", 160, UNITS1, 17, 3},
	{"ShootAssaultRifle", 70, UNITS1, 23, 1, 2, apply_damage, FixSmallHit},
	{"ShootRotaryCannon", 80, UNITS1, 23, 1, 3, apply_damage, FixSmallHit},
	{"Shoot20mm", 80, UNITS1, 23, 1, 4, apply_damage, FixSmallHit},
	{"Shoot30mm", 100, UNITS1, 23, 1, 5, apply_damage, FixSmallHit},
	{"Shoot155mm", 110, UNITS1, 24, 1, 7, apply_damage, FixMediumHit},
	{"Shoot200mm", 110, UNITS1, 25, 1, 8, apply_damage, FixLargeHit},
	{"FireRocket", 100, UNITS, 20, 5, 0, 0, FixExplosion},
	{"FixSmallHit", 0, UNITS1, 2, 1, 0, add_explosion_tile},
	{"FixMediumHit", 0, UNITS1, 3, 1, 0, add_explosion_tile},
	{"FixLargeHit", 0, UNITS1, 4, 1, 0, add_explosion_tile},
	{"FixExplosion", 0, UNITS1, 32, 5, 0, add_explosion_tile},
	{"FixBigExplosion", 0, UNITS1, 37, 5, 0, add_explosion_tile},
	{"FixBikeExplosion", 100, UNITS1, 32, 2, 0, 0, FixBikeExplosionEnd},
	{"FixBikeExplosionEnd", 100, UNITS1, 0, 2, 0, add_explosion_tile},
	{"FixHitSand", 200, UNITS1, 5, 3, 0, add_explosion_tile},
};
assert_enum(fixeffecti, FixHitSand)