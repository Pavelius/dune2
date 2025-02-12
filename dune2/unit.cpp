#include "area.h"
#include "bsdata.h"
#include "game.h"
#include "movement.h"
#include "rand.h"
#include "squad.h"
#include "unit.h"
#include "view.h"

BSDATAC(unit, 2048)
BSDATA(uniti) = {
	{"LightInfantry", Footed, UNITS, 0, 0, 90},
	{"HeavyInfantry", Footed, UNITS, 0, 0, 76},
	{"Trike", Wheeled, UNITS, 5, 0, 80, {6, 3, 2, 10}},
	{"Tank", Tracked, UNITS2, 0, 5, 78, {8, 5, 1, 5}},
	{"AssaultTank", Tracked, UNITS2, 10, 15, 72, {12, 6, 2, 4}},
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

void unit::blockunits() const {
	for(auto& e : bsdata<unit>()) {
		if(e && &e != this)
			path_map[e.position.y][e.position.x] = BlockArea;
	}
}

int unit::getspeed() const {
	auto n = get(Speed);
	if(!n)
		return 10;
	return 64 * 4 / n;
}

void unit::update() {
	auto v = m2sc(position);
	if(ismoveorder() || screen != v) { // We move somewhere
		if(screen == v) {
			// Determine path direction
			area.blockland(geti().move);
			blockunits();
			if(path_map[order.y][order.x] == BlockArea) {
				stop(); // Damn, someone block path. Remove order.
				return;
			}
			area.makewave(order, geti().move);
			path_direction = area.moveto(position, move_direction);
			shoot_direction = to(shoot_direction, turnto(shoot_direction, path_direction));
		}
		auto move_speed = getspeed();
		start_time += move_speed;
		if(move_direction != path_direction) {
			// Need turn to path direction. One correction per turn can be free.
			move_direction = to(move_direction, turnto(move_direction, path_direction));
			start_time += move_speed / 3;
		}
		if(move_direction == path_direction) {
			// Center, Up, RightUp, Right, RightDown, Down, LeftDown, Left, LeftUp,
			static point movesteps[LeftUp + 1] = {
				{0, 0}, {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1},
			};
			// Move to target point
			screen = screen + movesteps[move_direction];
			position = s2m(screen);
			if(isdiagonal(move_direction))
				start_time += move_speed / 3;
		}
	} else {
		// Random look around
		if(isturret()) {
			auto dt = turnto(shoot_direction, move_direction);
			if(dt != Center && d100() < 50)
				shoot_direction = to(shoot_direction, dt);
			else if(d100() < 30)
				shoot_direction = to(shoot_direction, (rand() % 2) ? Left : Right);
		}
	}
}

void unit::move(point v) {
	order = v;
	start_time = game.time; // Can't wait command
}

void unit::stop() {
	order = position;
	start_time += xrand(500, 1000); // Need some smoke.
}