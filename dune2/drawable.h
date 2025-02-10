#pragma once

#include "point.h"

enum drawtypen : unsigned char;

struct drawable {
	point			position;
	drawtypen		type;
	unsigned char	param;
};
struct draworder {
	unsigned short	index;
	drawtypen		type;
	point			order;
	unsigned long	start;
};