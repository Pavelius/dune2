#pragma once

#include "point.h"

enum direction : unsigned char {
	Center,
	Up, RightUp, Right, RightDown, Down, LeftDown, Left, LeftUp,
};

extern direction all_strait_directions[4];
extern direction all_diagonal_directions[4];
extern direction all_directions[8];

point getpoint(direction d);
direction to(direction d, direction s);
direction turnto(direction d, direction t);
direction to(point s, point d);
direction to(point s, point d);
unsigned char toh(point s, point d);
point transform(point v, direction d);
bool isdiagonal(direction d);