#pragma once

#include "point.h"
#include "adat.h"

struct pointa : adat<point, 512> {
	point	nearest(point n, int maximum_range = 0);
	void	selectfree();
};
