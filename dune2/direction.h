#pragma once

#include "point.h"

enum direction : unsigned char {
	Center,
	Up, RightUp, Right, RightDown, Down, LeftDown, Left, LeftUp,
};

point getpoint(direction d);
direction to(direction d, direction s);
direction turnto(direction d, direction t);
bool isdiagonal(direction d);