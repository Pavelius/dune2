#pragma once

#include "nameable.h"
#include "direction.h"
#include "point.h"

enum ordern : unsigned char {
	Stop, Move, Attack, Retreat, Harvest,
};

struct unit;

struct orderi : nameable {
};