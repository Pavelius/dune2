#pragma once

#include "point.h"

enum shapen : unsigned char {
	Shape1x1, Shape2x2, Shape3x2, Shape3x3
};
struct shapei {
	int		count;
	point	size;
	point	points[16];
	int		hits;
};