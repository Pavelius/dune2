#include "area.h"
#include "bsdata.h"
#include "squad.h"
#include "unit.h"
#include "view.h"

BSDATAC(unit, 2048)
BSDATA(uniti) = {
	{"LightInfantry", UNITS, 0, 0, 90},
	{"HeavyInfantry", UNITS, 0, 0, 76},
	{"Trike", UNITS, 5, 0, 80, {6}},
	{"Tank", UNITS2, 0, 5, 78, {8}},
	{"AssaultTank", UNITS2, 10, 15, 72, {12}},
};
assert_enum(uniti, AssaultTank)

unit *last_unit, *spot_unit;

const uniti& unit::geti() const {
	return bsdata<uniti>::elements[type];
}

playeri* unit::getplayer() const {
	if(player == 0xFF)
		return 0;
	return bsdata<playeri>::elements + player;
}

void unit::setplayer(const playeri* v) {
	if(v)
		player = v - bsdata<playeri>::elements;
	else
		player = 0xFF;
}

int	unit::getmaximum(statn v) const {
	switch(v) {
	case Hits: case Supply: return get(v) * 10;
	default: return 0;
	}
}

void unit::set(point v) {
	position = v;
	screen = m2sc(v);
}

void add_unit(point pt, direction d, unitn id, const playeri* player) {
	last_unit = bsdata<unit>::addz();
	last_unit->render = last_unit->renderindex();
	last_unit->screen = m2sc(pt);
	last_unit->position = pt;
	last_unit->order = pt;
	last_unit->type = id;
	last_unit->squad = NoSquad;
	last_unit->move_direction = d;
	last_unit->shoot_direction = d;
	last_unit->hits = last_unit->getmaximum(Hits);
	last_unit->supply = last_unit->getmaximum(Supply);
	last_unit->setplayer(player);
}

unit* find_unit(point v) {
	for(auto& e : bsdata<unit>()) {
		if(e && e.position == v)
			return &e;
	}
	return 0;
}

void blockunits(const unit* exclude) {
	for(auto& e : bsdata<unit>()) {
		if(e && &e != exclude)
			path_map[e.position.y][e.position.x] = BlockArea;
	}
}

direction unit::needmove() const {
	if(!ismoving())
		return Center;
	clearpath();
	flag32 flags;
	flags.set(Mountain);
	area.blockland(flags);
	blockunits(this);
	area.makewave(order);
	return area.moveto(position, move_direction);
}