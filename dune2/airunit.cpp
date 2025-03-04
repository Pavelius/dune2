#include "airunit.h"
#include "area.h"
#include "bsdata.h"
#include "building.h"
#include "game.h"

BSDATAC(airunit, 64)

void add_air_unit(point pt, objectn id, direction d, unsigned char player) {
	if(!area.isvalid(pt))
		return;
	auto p = bsdata<airunit>::addz();
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

void airunit::update() {
	const auto move = Flying;
	auto speed = getspeedfp(getspeed(type));
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

void airunit::patrol() {
	auto pb = find_base(player);
	auto pt = pb->position;
	rect rc = {pt.x - 8, pt.y - 8, pt.x + 8, pt.y + 8};
	//order = area.random(area.regions[getplayer().region]);
	order = area.random(area_correct(rc));
}