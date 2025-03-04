#include "area.h"
#include "bsdata.h"
#include "unita.h"
#include "view_map.h"

unita human_selected;

void unita::select(unsigned char player_index, const rect& screen_area) {
	for(auto& e : bsdata<unit>()) {
		if(!e || e.player != player_index)
			continue;
		if(e.screen.in(screen_area))
			add(&e);
	}
}

rect unita::selectrect() const {
	auto p = first();
	if(!p)
		return {-1, -1, -1, -1};
	rect rc = {p->position.x, p->position.y, p->position.x, p->position.y};
	for(auto p : *this) {
		if(p->position.x < rc.x1)
			rc.x1 = p->position.x;
		if(p->position.x > rc.x2)
			rc.x2 = p->position.x;
		if(p->position.y < rc.y1)
			rc.y1 = p->position.y;
		if(p->position.y > rc.y2)
			rc.y2 = p->position.y;
	}
	return rc;
}

static point choose_terrain() {
	return show_scene(paint_main_map_choose_terrain, 0, 0);
}

void unita::order(ordern type, point v, bool interactive) const {
	if(!operator bool())
		return;
	if(interactive && !area.isvalid(v)) {
		switch(type) {
		case Move: case Attack: case Harvest:
			v = choose_terrain();
			if(!area.isvalid(v))
				return;
			break;
		}
	}
	for(auto p : *this)
		p->setorder(type, v);
	hilite_unit_orders();
}

void unita::order(point v) const {
	if(!operator bool())
		return;
	for(auto p : *this)
		p->setorder(v);
	hilite_unit_orders();
}