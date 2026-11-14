#pragma once

#include "terrain.h"

enum movementn : unsigned char {
	NoMovement, Footed, Wheeled, Tracked, Undersand, Flying,
	LastMovement = Flying
};
extern unsigned char move_cost[LastMovement + 1][Mountain + 1];
