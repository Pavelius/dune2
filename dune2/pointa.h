#pragma once

#include "area.h"
#include "point.h"
#include "adat.h"

struct pointa : adat<point, 512> {
	point nearest(point n, int maximum_range = 0);
	void filter(unsigned char player, areaf f, bool keep);
	void select(const rect& rc, areai::fntest proc);
	void select(const rect& rc, areaf id, unsigned char player, bool value);
	void selectfree();
};
