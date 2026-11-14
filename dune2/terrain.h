#pragma once

#include "color.h"
#include "flagable.h"

enum terrainn : unsigned char {
	Sand, Dune, Spice, SpiceRich, SpiceBlow, Rock, Mountain,
	LastTerrain = Mountain,
};
enum movementn : unsigned char {
	NoMovement, Footed, Wheeled, Tracked, Undersand, Flying,
	LastMovement = Flying
};

extern const char* terrain_names[LastTerrain + 1];
extern unsigned char move_cost[LastMovement + 1][Mountain + 1];

struct terraini {
	unsigned short frame, count;
	color	minimap;
	flag16	terrain; // Appear on this terrain only if filled
	flag16	same; // Same terrain when draw
};

inline const char* getnm(terrainn v) { return terrain_names[v]; }