#pragma once

#include "nameable.h"
#include "terrain.h"

enum movementn : unsigned char {
	Footed, Wheeled, Tracked, Flying
};
struct movementi : nameable {
	unsigned char cost[Mountain + 1];
};
