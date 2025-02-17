#include "area.h"
#include "unita.h"
#include "view.h"

unita human_selected;

void unita::select(const playeri* player, const rect& screen_area) {
	auto player_index = player->getindex();
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

void unita::order(ordern type, direction d, point v, bool interactive) const {
	if(interactive && !area.isvalid(v)) {
		switch(type) {
		case Move: case Attack:
			v = choose_terrain();
			if(!area.isvalid(v))
				return;
			break;
		}
	}
	if(d==Center && area.isvalid(v))
		d = to(center(human_selected.selectrect()), v);
	auto index = 0;
	for(auto p : human_selected) {
		if(area.isvalid(v)) {
			auto vt = formation(index++);
			vt = v + transform(vt, d);
			vt = area.nearest(vt, isfreetrack, 4);
			if(!vt)
				continue;
			p->apply(type, vt);
		} else
			p->apply(type, v);
	}
	if(type==Move)
		hilite_unit_orders();
}