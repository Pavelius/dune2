#include "bsdata.h"
#include "movement.h"

// Sand, Dune, Spice, SpiceRich, SpiceBlow, Rock, Mountain,
BSDATA(movementi) = {
	{"NoMovement"},
	{"Footed", {4, 5, 4, 4, 5, 4, 6}},
	{"Wheeled", {4, 4, 4, 4, 5, 8, 0xFF}},
	{"Tracked", {5, 5, 5, 5, 5, 4, 0xFF}},
	{"Undersand", {5, 5, 5, 5, 5, 0xFF, 0xFF}},
	{"Flying", {4, 4, 4, 4, 4, 4, 4}},
};