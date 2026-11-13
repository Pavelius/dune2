#pragma once

#include "color.h"
#include "flagable.h"
#include "nameable.h"

enum terrainn : unsigned char {
	Sand, Dune, Spice, SpiceRich, SpiceBlow, Rock, Mountain,
	LastTerrain = Mountain,
};
extern const char* terrain_names[LastTerrain + 1];

struct terraini : nameable {
	unsigned short	frame, count;
	color			minimap;
	flag16			terrain; // Appear on this terrain only if filled
	flag16			same; // Same terrain when draw
};

inline const char* getnm(terrainn v) { return terrain_names[v]; }