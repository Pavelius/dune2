#include "area.h"
#include "bsdata.h"
#include "drawable.h"
#include "fix.h"
#include "resid.h"
#include "view.h"

static void add_explosion_tile() {
	area.set(s2m(last_object->screen), Explosion);
}

void add_effect(point v, fixn i) {
	add_effect(v, i, animate_time);
}

BSDATA(fixeffecti) = {
	{"NoEffect"},
	{"FixExplosion", 0, UNITS1, 32, 5, add_explosion_tile},
	{"FixBigExplosion", 0, UNITS1, 37, 5, add_explosion_tile},
	{"FixBikeExplosion", 100, UNITS1, 32, 2, 0, FixBikeExplosionEnd},
	{"FixBikeExplosionEnd", 100, UNITS1, 0, 2, add_explosion_tile},
	{"FixHitSand", 200, UNITS1, 5, 3, add_explosion_tile},
};
assert_enum(fixeffecti, FixHitSand)