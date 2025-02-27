#include "area.h"
#include "bsdata.h"
#include "game.h"
#include "objective.h"
#include "pointa.h"
#include "unit.h"

BSDATAC(objective, 512)

static objective* find_objective(unit* p) {
	auto n = p->getindex();
	for(auto& e : bsdata<objective>()) {
		if(e.executor == n)
			return &e;
	}
	return 0;
}

static short unsigned find_executor(unsigned char player, statn best) {
	auto result_value = 0;
	unit* result = 0;
	for(auto& e : bsdata<unit>()) {
		if(!e || e.isboard() || e.player != player)
			continue;
		if(find_objective(&e))
			continue;
		auto value = e.get(best);
		if(result_value < value) {
			result_value = value;
			result = &e;
		}
	}
	return result ? result - bsdata<unit>::elements : 0xFFFF;
}

static bool load_executor(objective* p) {
	if(p->executor == 0xFFFF)
		last_unit = 0;
	else
		last_unit = bsdata<unit>::elements + p->executor;
	return last_unit != 0;
}

static bool assign_executor(objective* p, statn best) {
	if(p->executor == 0xFFFF)
		p->executor = find_executor(p->player, best);
	return load_executor(p);
}

static objective* find_objective(objectn type, unsigned char player, short unsigned param) {
	for(auto& e : bsdata<objective>()) {
		if(e.type == type && e.player == player && e.param == param)
			return &e;
	}
	return 0;
}

objective* add_objective(objectn type, unsigned char player, short unsigned param, short unsigned count) {
	auto p = find_objective(type, player, param);
	if(!p) {
		p = bsdata<objective>::addz();
		p->type = type;
		p->player = player;
		p->param = param;
		p->count = 0;
		p->executor = 0xFFFF;
		p->parent = 0xFFFF;
		p->turn = game.turn;
	}
	if(p->count < count)
		p->count = count;
	return p;
}

short unsigned objective::getindex() const {
	if(!this)
		return 0xFFFF;
	return this - bsdata<objective>::elements;
}

objective* objective::getparent() const {
	return (parent == 0xFFFF) ? 0 : bsdata<objective>::elements + parent;
}

//static bool assign_build(objective* p, objectn type) {
//	auto build = getbuild(type);
//	if(build) {
//		if(!player_active->objects[build]) {
//			p->setparent(add_objective(BuildStructure, p->player, build, 1));
//			return false;
//		}
//	}
//	return true;
//}
//
//static bool assign_require(objective* p, objectn type) {
//	auto required = getrequired(type);
//	if(required) {
//		if(!player_active->objects[required]) {
//			p->setparent(add_objective(BuildStructure, p->player, required, 1));
//			return false;
//		}
//	}
//	return true;
//}

/*	void objective::thinking() {
	switch(type) {
	case ExploreArea:
		if(!assign_executor(this, Speed))
			return;
		if(last_unit->isorder())
		return;
		points.clear();
		points.select(area.regions[param], Explored, player, false);
		if(!points)
			done();
		else
			last_unit->order = points.random();
		break;
	case BuildUnit:
		if(player_active->objects[param] >= count)
			done();
		if(!assign_build(this, (objectn)param))
			return;
		if(!assign_require(this, (objectn)param))
			return;
		break;
	case BuildStructure:
		if(player_active->objects[param] >= count)
			done();
		if(!assign_require(this, (objectn)param))
			return;
		break;
	default:
		return;
	}
}

void objective::update() {
	auto push_unit = last_unit;
	auto push_player = player_active;
	player_active = bsdata<playeri>::elements + player;
	thinking();
	player_active = push_player;
	last_unit = push_unit;
}*/

void objective::clear() {
	memset(this, 0, sizeof(*this));
}

void objective::cleanup() {
	auto i = getindex();
	for(auto& e : bsdata<objective>()) {
		if(e.parent == i)
			e.parent = 0xFFFF;
	}
}

void objective::done() {
	cleanup();
	clear();
}

void objective::setparent(const objective* v) {
	parent = v ? v - bsdata<objective>::elements : 0xFFFF;
}