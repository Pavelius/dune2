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

unit* add_unit(point pt, direction d, unitn id, const playeri* player) {
	auto p = bsdata<unit>::addz();
	p->render = p->renderindex();
	p->screen = m2sc(pt);
	p->type = id;
	p->set(d);
	p->hits = p->getmaximum(Hits);
	p->supply = p->getmaximum(Supply);
	p->setplayer(player);
	return p;
}