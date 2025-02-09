#pragma once

#include "nameable.h"
#include "pointc.h"

enum shapen : unsigned char {
	Shape1x1, Shape2x2, Shape3x2,
};
struct shapei : nameable {
	int			count;
	pointc		size;
	pointc		points[16];
};