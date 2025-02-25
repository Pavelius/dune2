#pragma once

#include "adat.h"
#include "unit.h"

struct unita : adat<unit*> {
	void order(ordern type, point v, bool interactive) const;
	void order(point v) const;
	void select(unsigned char player_index, const rect& screen_area);
	rect selectrect() const;
};
extern unita human_selected;