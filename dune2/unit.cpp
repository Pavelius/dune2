#include "area.h"
#include "bsdata.h"
#include "unit.h"
#include "view.h"

BSDATAC(unit, 2048)
BSDATA(uniti) = {
	{"LightInfantry", UNITS},
	{"HeavyInfantry", UNITS},
	{"Trike", UNITS, 5, 0, {6}},
	{"Tank", UNITS2, 0, 5, {8}},
	{"AssaultTank", UNITS2, 10, 15, {12}},
};
assert_enum(uniti, AssaultTank)

unit* last_unit;

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

void add_unit(point pt, direction d, unitn id, const playeri* player) {
	last_unit = bsdata<unit>::addz();
	last_unit->render = last_unit->renderindex();
	last_unit->screen = m2sc(pt);
	last_unit->position = pt;
	last_unit->order = pt;
	last_unit->type = id;
	last_unit->move_direction = d;
	last_unit->shoot_direction = d;
	last_unit->hits = last_unit->getmaximum(Hits);
	last_unit->supply = last_unit->getmaximum(Supply);
	last_unit->setplayer(player);
}