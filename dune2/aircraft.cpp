#include "aircraft.h"
#include "area.h"
#include "bsdata.h"
#include "building.h"
#include "game.h"
#include "unit.h"

BSDATAC(aircraft, 64)

void add_air_unit(point pt, objectn id, direction d, unsigned char player) {
	if(!area.isvalid(pt))
		return;
	auto p = bsdata<aircraft>::addz();
	p->render = p->renderindex();
	p->screen = m2sc(pt);
	p->position = pt;
	p->type = id;
	p->move_direction = d;
	p->shoot_direction = d;
	p->hits = last_unit->gethitsmax();
	p->player = player;
	p->start_time = game.time;
	p->stop();
}

static building* find_base(unsigned char player) {
	auto p = find_base(HighTechFacility, player);
	if(!p)
		p = find_base(ConstructionYard, player);
	return p;
}

static unit* get_unit(short unsigned n) {
	return bsdata<unit>::elements + n;
}

void aircraft::clear() {
	memset(this, 0, sizeof(*this));
}

void aircraft::destroy() {
	cleanup(true);
	clear();
}

void aircraft::cleanup(bool destroying) {
	for(auto n : load) {
		auto p = get_unit(n);
		if(destroying)
			p->position = position;
		p->destroy();
	}
}

int	aircraft::getindex() const {
	return this - bsdata<aircraft>::elements;
}

void aircraft::update() {
	const auto move = Flying;
	auto speed = getspeedfp(getstat(type, Speed));
	if(moving(move, speed, 0))
		return;
	else if(nextmoving(move, speed))
		return;
	else {
		patrol();
		nextmoving(move, speed);
	}
}

static rect area_correct(rect rc) {
	if(rc.x1 < 0)
		rc.x1 = 0;
	if(rc.x2 >= area.maximum.x)
		rc.x2 = area.maximum.x - 1;
	if(rc.y1 < 0)
		rc.y1 = 0;
	if(rc.y2 >= area.maximum.y)
		rc.y2 = area.maximum.y - 1;
	return rc;
}

point aircraft::getstarbase(point v) const {
	if(getindex() % 2) {
		if(v.x < area.maximum.x / 2)
			v.x = 0;
		else
			v.x = area.maximum.x - 1;
	} else {
		if(v.y < area.maximum.y / 2)
			v.y = 0;
		else
			v.y = area.maximum.y - 1;
	}
	return v;
}

void aircraft::leave() {
	cleanup(false);
	clear();
}

void aircraft::returnbase() {
	auto pb = find_base(player);
	if(!pb)
		return;
	auto pt = getstarbase(pb->position);
	if(position == pt)
		leave();
	else
		order = pt;
}

void aircraft::patrol() {
	auto pb = find_base(player);
	if(!pb)
		return;
	auto pt = pb->position;
	rect rc = {pt.x - 8, pt.y - 8, pt.x + 8, pt.y + 8};
	switch(type) {
	case Fregate:
		returnbase();
		break;
	case Carrier:
		if(action++ >= 4)
			returnbase();
		else
			order = area.random(area_correct(rc));
		break;
	default:
		order = area.random(area_correct(rc));
		break;
	}
}