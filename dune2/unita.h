#pragma once

#include "collection.h"
#include "unit.h"

struct unita : collection<unit> {
	void order(ordern type, direction d, point v, bool interactive) const;
	void select(const playeri* player, const rect& screen_area);
	rect selectrect() const;
};
extern unita human_selected;