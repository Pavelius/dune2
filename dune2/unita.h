#pragma once

#include "collection.h"
#include "order.h"
#include "unit.h"

struct unita : collection<unit> {
	void order(ordern type, direction d, point v, unit* target, bool interactive, bool autotarget) const;
	void select(const playeri* player, const rect& screen_area);
	rect selectrect() const;
};
extern unita human_selected;