#include "area.h"
#include "bsdata.h"
#include "building.h"
#include "fix.h"
#include "game.h"
#include "math.h"
#include "movement.h"
#include "print.h"
#include "player.h"
#include "stringbuilder.h"
#include "squad.h"
#include "unit.h"
#include "unita.h"
#include "view.h"

BSDATAC(unit, 2048)
BSDATA(uniti) = {
	{"Harvester", HARVEST, 88, 400, Tracked, NoEffect, UNITS, 10, 0, {10, 0, 4, 4}},
	{"LightInfantry", INFANTRY, 81, 40, Footed, ShootAssaultRifle, UNITS, 91, 0, {4, 1, 2, 0}},
	{"HeavyInfantry", HYINFY, 91, 70, Footed, ShootRotaryCannon, UNITS, 103, 0, {4, 1, 3, 1}},
	{"Trike", TRIKE, 80, 100, Wheeled, Shoot20mm, UNITS, 5, 0, {5, 2, 10, 1}},
	{"Quad", QUAD, 74, 140, Wheeled, Shoot30mm, UNITS, 0, 0, {5, 2, 8, 2}},
	{"Tank", LTANK, 78, 200, Tracked, Shoot155mm, UNITS2, 0, 5, {6, 1, 5, 2}},
	{"AssaultTank", HTANK, 72, 350, Tracked, Shoot155mm, UNITS2, 10, 15, {6, 2, 4, 3}},
};
assert_enum(uniti, AssaultTank)

unit *last_unit;

const unsigned long shoot_duration = 1000 * 3;
const unsigned long shoot_next_attack = 200;

point formation(int index) {
	static point formations[] = {
		{0, 0}, {1, 0}, {-1, 0}, {2, 0}, {-2, 0},
		{0, 1}, {1, 1}, {-1, 1}, {2, 1}, {-2, 1},
		{0, 2}, {1, 2}, {-1, 2}, {2, 2}, {-2, 2},
	};
	index %= sizeof(formations) / sizeof(formations[0]);
	return formations[index];
}

static bool turn(direction& result, direction new_direction) {
	if(new_direction == Center)
		return false;
	if(result != new_direction)
		result = to(result, turnto(result, new_direction));
	return result == new_direction;
}

void unit::fixstate(const char* id) const {
	auto push = last_unit; last_unit = const_cast<unit*>(this);
	actable::fixstate(id);
	last_unit = push;
}

void unit::destroy() {
	fixstate("UnitDestroyed");
	cleanup();
	switch(geti().move) {
	case Wheeled: add_effect(m2sc(position), FixBikeExplosion); break;
	case Tracked: add_effect(m2sc(position), FixExplosion); break;
	}
	clear();
}

void unit::damage(int value) {
	if(hits > value)
		hits -= value;
	else
		destroy();
}

bool unit::ismoving() const {
	return !isboard() && screen != m2sc(position);
}

short unsigned unit::getindex() const {
	return this - bsdata<unit>::elements;
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

void unit::cleanup() {
	auto i = getindex();
	for(auto& e : bsdata<unit>()) {
		if(!e)
			continue;
		if(e.target == i)
			e.actable::stop();
	}
	if(last_unit == this)
		last_unit = 0;
	human_selected.remove(this);
}

void unit::scouting() {
	if(position.x < 0)
		return;
	area.scouting(position, player, getlos());
}

void blockunits() {
	for(auto& e : bsdata<unit>()) {
		if(e && !e.isboard())
			path_map[e.position.y][e.position.x] = BlockArea;
	}
}

int unit::getspeed() const {
	auto n = get(Speed);
	if(!n)
		return 64 * 4 * 2;
	return 64 * 4 / n;
}

ordern unit::getpurpose() const {
	switch(type) {
	case Harvester: return Harvest;
	default: return Attack;
	}
}

static point random_near(point v) {
	return to(v, all_directions[game_rand() % (sizeof(all_directions) / sizeof(all_directions[0]))]);
}

bool unit::isharvest() const {
	return getpurpose() == Harvest && iswork();
}

bool unit::istrallfull() const {
	return getpurpose() == Harvest && action >= 10;
}

bool unit::releasetile() {
	auto p = find_unit(position, this);
	if(!p)
		return false;
	for(auto d : all_directions) {
		auto v = to(position, d);
		if(!area.isvalid(v))
			continue;
		auto p = find_unit(v);
		if(!p) {
			move_direction = d;
			startmove(getspeed());
			order = position;
			return true;
		}
	}
	start_time += 1000;
	return true;
}

bool unit::harvest() {
	if(getpurpose() != Harvest)
		return false;
	if(isboard()) {
		auto pb = find_board(this);
		if(!pb || pb->type != Refinery)
			return false;
		if(action) {
			action--;
			pb->getplayer().add(Credits, 100);
			start_time += 1000;
		} else {
			pb->unboard();
			action_time = 0;
			if(area.isvalid(target_position))
				apply(Move, target_position);
		}
		return true;
	} else if(!area.isvalid(target_position))
		return false;
	if(istrallfull()) {
		returnbase();
		return true;
	}
	auto v = area.nearest(position, isspicefield, 4 + getlos());
	if(!area.isvalid(v)) {
		stop();
		return false;
	}
	if(position == v) {
		action++;
		start_time += 1000 * 4;
		action_time = start_time;
		switch(area.get(position)) {
		case Spice:
			area.set(position, Sand);
			break;
		case SpiceRich:
			area.set(position, Spice);
			action++;
			break;
		}
		fixstate("HarvesterWork");
	} else {
		action_time = 0;
		apply(Move, v);
	}
	return true;
}

bool unit::shoot() {
	return actable::shoot(screen, geti().weapon, get(Attacks), getshootrange());
}

bool unit::seeking() {
	if(game_chance(30)) {
		if(isturret()) { // Turret random look around
			auto turn_direction = turnto(action_direction, move_direction);
			if(turn_direction != Center && game_chance(50))
				action_direction = to(action_direction, turn_direction);
			else if(game_chance(30))
				action_direction = to(action_direction, (game_rand() % 2) ? Left : Right);
		}
	}
	return false;
}

void unit::acting() {
	if(ismoving()) {
		if(isturret())
			shoot();
	} else if(shoot()) {
		if(!isturret())
			move_direction = action_direction;
		return;
	}
}

void unit::update() {
	if(moving(geti().move, getspeed(), getlos())) {
		if(!isturret())
			action_direction = move_direction;
		else if(!area.isvalid(target_position)) {
			turn(action_direction, move_direction);
			animate_time = game.time + look_duration / 2;
		}
		return;
	} else if(releasetile())
		return;
	else if(harvest())
		return;
	else if(seeking())
		return;
}

void unit::stop() {
	path_direction = Center;
	order = position;
	actable::stop();
}

bool isnonblocked(point v) {
	return path_map[v.y][v.x] != BlockArea;
}

bool isunitpossible(point v) {
	auto f = area.getfeature(v);
	return f != BuildingHead && f != BuildingLeft && f != BuildingUp;
}

bool isfreetrack(point v) {
	return isunitpossible(v) && !area.is(v, Mountain);
}

bool isfreefoot(point v) {
	return isunitpossible(v);
}

bool isspicefield(point v) {
	auto t = area.get(v);
	return (t == Spice || t == SpiceRich);
}

unit* find_unit(point v) {
	for(auto& e : bsdata<unit>()) {
		if(e && e.position == v)
			return &e;
	}
	return 0;
}

unit* find_unit(point v, const unit* exclude) {
	for(auto& e : bsdata<unit>()) {
		if(e && &e != exclude && e.position == v)
			return &e;
	}
	return 0;
}

void add_unit(point pt, unitn id, direction d) {
	pt = area.nearest(pt, isfreetrack, 4);
	if(!area.isvalid(pt))
		return;
	last_unit = bsdata<unit>::addz();
	last_unit->render = last_unit->renderindex();
	last_unit->screen = m2sc(pt);
	last_unit->position = pt;
	last_unit->order = pt;
	last_unit->type = id;
	last_unit->squad = NoSquad;
	last_unit->move_direction = d;
	last_unit->action_direction = d;
	last_unit->path_direction = Center;
	last_unit->hits = last_unit->getmaximum(Hits);
	last_unit->target = 0xFFFF;
	last_unit->target_position = {-10000, -10000};
	last_unit->setplayer(player);
	last_unit->scouting();
}

void unit::clear() {
	memset(this, 0, sizeof(*this));
}

void unit::cantdothis() {
	start_time += game_rand(4, 8) * 1000;
	print("%1 can't do nothing.", getname());
}

bool unit::returnbase() {
	auto kind = getpurpose();
	if(kind == Harvest) {
		action_time = 0;
		auto pb = find_base(Refinery, player);
		if(!pb) {
			cantdothis(); // Something wrong
			return false; // Not any valid base present
		}
		blockland(geti().move);
		blockunits();
		unblock();
		area.movewave(pb->position, geti().move, pb->getsize()); // Consume time action
		auto v = find_smallest_position();
		if(!area.isvalid(v)) {
			cantdothis(); // Something wrong
			return false;
		}
		if(position == v) {
			fixstate("HarvesterUnload");
			pb->board(this);
		} else {
			fixstate("HarvesterReturn");
			apply(Move, v);
		}
		return true;
	} else {
		auto pb = find_base(ConstructionYard, player);
		if(!pb)
			pb = find_base(HeavyVehicleFactory, player);
		if(!pb)
			pb = find_base(RadarOutpost, player);
		if(!pb) {
			cantdothis(); // Something wrong
			stop();
			return false; // Not any valid base present
		}
		blockland(geti().move);
		blockunits();
		unblock();
		area.movewave(pb->position, geti().move, pb->getsize()); // Consume time action
		auto v = find_smallest_position();
		if(!area.isvalid(v)) {
			cantdothis(); // Something wrong - path is blocking
			return false;
		}
		if(position == v)
			stop(); // Just arrived to final place
		else
			apply(Move, v);
	}
	return false;
}

void unit::apply(ordern type, point v) {
	unit* opponent;
	switch(type) {
	case Attack: setaction(v, true); break;
	case Harvest: setaction(v, false); break;
	case Move: order = v; start_time = game.time; break;
	case SmartMove:
		opponent = find_unit(v);
		if(opponent && opponent->isenemy(player) && getpurpose() == Attack)
			apply(Attack, v);
		else if(getpurpose() == Harvest && isspicefield(v))
			apply(Harvest, v);
		else if(area.isvalid(v))
			apply(Move, v);
		break;
	case Retreat: returnbase(); break;
	default: stop(); break;
	}
}