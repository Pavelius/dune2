#pragma once

#include "point.h"
#include "adat.h"

struct pointa : adat<point, 512> {
	void	selectfree();
};
