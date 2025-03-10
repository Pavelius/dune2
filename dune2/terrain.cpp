#include "bsdata.h"
#include "terrain.h"
#include "math.h"

BSDATA(terraini) = {
	{"Sand", 127, 1, color(180,120,56), 0, FG(Dune) | FG(Spice) | FG(SpiceRich) | FG(SpiceBlow)},
	{"Dune", 144, 16, color(196, 134, 64), FG(Sand)},
	{"Spice", 176, 16, color(168, 72, 24), FG(Sand) | FG(Dune), FG(SpiceRich)},
	{"SpiceRich", 192, 16, color(140, 44, 12), FG(Sand) | FG(Dune) | FG(Spice)},
	{"SpiceBlow", 208, 2, color(0, 0, 0), FG(Sand) | FG(Dune) | FG(Spice) | FG(SpiceRich)},
	{"Rock", 128, 16, color(80, 80, 60), 0, FG(Mountain)},
	{"Mountain", 160, 16, color(48, 28, 0), FG(Rock)},
};
assert_enum(terraini, Mountain)