#pragma once

#include "player.h"
#include "collection.h"
#include "unit.h"

struct unita : collection<unit> {
	void select(const playeri* player, const rect& screen_area);
	rect selectrect() const;
};
extern unita human_selected;