#include "bsdata.h"
#include "movement.h"

// Sand, Dune, Spice, SpiceRich, SpiceBlow, Rock, Mountain,

BSDATA(movementi) = {
	{"Footed", {4, 5, 4, 4, 5, 4, 10}},
	{"Wheeled", {4, 4, 4, 4, 5, 6, 0xFF}},
	{"Tracked", {5, 5, 5, 5, 5, 4, 0xFF}},
	{"Flying", {1, 1, 1, 1, 1, 1, 1}},
};