#include "aircraft.h"
#include "area.h"
#include "bsdata.h"
#include "building.h"
#include "game.h"
#include "unit.h"

BSDATAC(aircraft, 64)

aircraft* last_aircraft;

static building* find_base(unsigned char player) {
	auto p = find_base(HighTechFactory, player);
	if(!p)
		p = find_base(ConstructionYard, player);
	return p;
}

static point random_base(unsigned char player) {
	point v;
	auto p = find_base(player);
	if(p)
		v = p->position;
	else
		v = area.center();
	v.x += game_rand() % 4 - 2;
	v.y += game_rand() % 4 - 2;
	return v;
}

void add_air_unit(point pt, objectn id, direction d, unsigned char player) {
	if(!area.isvalid(pt))
		return;
	last_aircraft = bsdata<aircraft>::addz();
	last_aircraft->clear();
	last_aircraft->render = last_aircraft->renderindex();
	last_aircraft->screen = m2sc(pt);
	last_aircraft->position = pt;
	last_aircraft->type = id;
	last_aircraft->move_direction = d;
	last_aircraft->shoot_direction = d;
	last_aircraft->hits = last_unit->gethitsmax();
	last_aircraft->player = player;
	last_aircraft->start_time = game.time;
	last_aircraft->stop();
}

void add_unit_by_air(point pt, objectn id, direction d, unsigned char player) {
	add_unit(pt, id, d, player);
	auto start = get_star_base(random_base(player), bsdata<aircraft>::source.count);
	add_air_unit(start, Carryall, to(start, area.center()), player);
	last_aircraft->board(last_unit);
	last_aircraft->order = pt;
}

static unit* get_unit(short unsigned n) {
	if(n == 0xFFFF)
		return 0;
	return bsdata<unit>::elements + n;
}

void aircraft::clear() {
	memset(this, 0, sizeof(*this));
	cargo = 0xFFFF;
}

void aircraft::board(unit* p) {
	cargo = p->getindex();
	p->position.x = -100;
	p->position.y = getindex();
}

void aircraft::unboard() {
	auto p = get_unit(cargo);
	if(!p)
		return;
	cargo = 0xFFFF;
	p->position = position;
	p->screen = m2sc(p->position);
	p->stop();
}

void aircraft::destroy() {
	cleanup(true);
	clear();
}

void aircraft::cleanup(bool destroying) {
	if(destroying)
		unboard();
	auto p = get_unit(cargo);
	if(p)
		p->destroy();
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

point get_star_base(point v, int seed) {
	if(seed % 2) {
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

point get_star_base(unsigned char player) {
	point pt;
	auto pb = find_base(player);
	if(pb)
		pt = pb->position;
	else
		pt = area.center();
	return get_star_base(pt, pt.x);
}

void aircraft::leave() {
	cleanup(false);
	clear();
}

void aircraft::returnbase() {
	auto pb = find_base(player);
	if(!pb)
		return;
	auto pt = get_star_base(pb->position, getindex());
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
	case Carryall:
		if(iscargo())
			unboard();
		returnbase();
		break;
	default:
		order = area.random(area_correct(rc));
		break;
	}
}