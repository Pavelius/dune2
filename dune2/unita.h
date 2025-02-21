#pragma once

#include "collection.h"
#include "unit.h"

struct unita : collection<unit> {
	void order(ordern type, point v, bool interactive) const;
	void select(unsigned char player_index, const rect& screen_area);
	rect selectrect() const;
};
extern unita human_selected;