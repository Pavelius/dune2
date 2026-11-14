#include "bsdata.h"
#include "terrain.h"
#include "math.h"

BSDATA(terraini) = {
	{127, 1, color(180,120,56), 0, FG(Dune) | FG(Spice) | FG(SpiceRich) | FG(SpiceBlow)}, // Sand
	{144, 16, color(196, 134, 64), FG(Sand)}, // Dune
	{176, 16, color(168, 72, 24), FG(Sand) | FG(Dune), FG(SpiceRich)}, // Spice
	{192, 16, color(140, 44, 12), FG(Sand) | FG(Dune) | FG(Spice)}, // SpiceRich
	{208, 2, color(0, 0, 0), FG(Sand) | FG(Dune) | FG(Spice) | FG(SpiceRich)}, // SpiceBlow
	{128, 16, color(80, 80, 60), 0, FG(Mountain)}, // Rock
	{160, 16, color(48, 28, 0), FG(Rock)}, // Mountain
};
assert_enum(terraini, Mountain)

// Sand, Dune, Spice, SpiceRich, SpiceBlow, Rock, Mountain
unsigned char move_cost[LastMovement + 1][Mountain + 1] = {
	{},
	{4, 5, 4, 4, 5, 4, 6}, // Footed
	{4, 4, 4, 4, 5, 8, 0xFF}, // Wheeled
	{5, 5, 5, 5, 5, 4, 0xFF}, // Tracked
	{5, 5, 5, 5, 5, 0xFF, 0xFF}, // Undersand
	{4, 4, 4, 4, 4, 4, 4}, // Flying
};