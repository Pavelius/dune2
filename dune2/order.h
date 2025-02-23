#pragma once

#include "nameable.h"

enum ordern : unsigned char {
	Stop, Move, MoveAndAttack, Attack, Retreat, Harvest,
};
struct orderi : nameable {
};