#pragma once

#include "flagable.h"
#include "nameable.h"

enum terrainn : unsigned char {
	Sand, Dune, Spice, SpiceRich, SpiceBlow, Rock, Mountain,
};
enum terrainf : unsigned char {
	Impassable, SlowFootMovement, SlowTrackMovement, SlowWheelMovement,
};
struct terraini : nameable {
	unsigned short	frame, count;
	flag8			flags;
	flag16			terrain; // Appear on this terrain only if filled
	flag16			same; // Same terrain when draw
	bool			is(terrainf v) const { return flags.is(v); }
};