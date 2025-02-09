#include "bsdata.h"
#include "terrain.h"
#include "math.h"

BSDATA(terraini) = {
	{"Sand", 127, 1, 0, 0, FG(Dune) | FG(Spice) | FG(SpiceRich) | FG(SpiceBlow)},
	{"Dune", 144, 16, FG(SlowTrackMovement), FG(Sand)},
	{"Spice", 176, 16, FG(SlowTrackMovement), FG(Sand) | FG(Dune), FG(SpiceRich)},
	{"SpiceRich", 192, 16, FG(SlowTrackMovement), FG(Sand) | FG(Dune) | FG(Spice)},
	{"SpiceBlow", 208, 2, FG(SlowTrackMovement), FG(Sand) | FG(Dune) | FG(Spice) | FG(SpiceRich)},
	{"Rock", 128, 16, FG(SlowWheelMovement), 0, FG(Mountain)},
	{"Mountain", 160, 16, FG(Impassable), FG(Rock)},
};
assert_enum(terraini, Mountain)