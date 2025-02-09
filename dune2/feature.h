#pragma once

#include "flagable.h"
#include "nameable.h"

enum featuren : unsigned char {
	NoFeature,
	Explosion, AircraftRemains, CarRemains, StructureRemains, Body, Bodies, Blood, Trail,
	Slab, Wall,
	BuildingHead, BuildingLeft, BuildingUp,
	FogOfWar
};
struct featurei : nameable {
	unsigned short	frame, count;
	unsigned char	decoy;
};