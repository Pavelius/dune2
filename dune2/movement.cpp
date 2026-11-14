#include "movement.h"

// Sand, Dune, Spice, SpiceRich, SpiceBlow, Rock, Mountain
unsigned char move_cost[LastMovement + 1][Mountain + 1] = {
	{},
	{4, 5, 4, 4, 5, 4, 6}, // Footed
	{4, 4, 4, 4, 5, 8, 0xFF}, // Wheeled
	{5, 5, 5, 5, 5, 4, 0xFF}, // Tracked
	{5, 5, 5, 5, 5, 0xFF, 0xFF}, // Undersand
	{4, 4, 4, 4, 4, 4, 4}, // Flying
};