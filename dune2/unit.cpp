#include "area.h"
#include "bsdata.h"
#include "game.h"
#include "movement.h"
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

unit *last_unit;

point formation(int index) {
	static point formations[] = {
		{0, 0}, {1, 0}, {-1, 0}, {2, 0}, {-2, 0}, {3, 0}, {-3, 0},
		{0, 1}, {1, 1}, {-1, 1}, {2, 1}, {-2, 1}, {3, 1}, {-3, 1},
		{0, 2}, {1, 2}, {-1, 2}, {2, 2}, {-2, 2}, {3, 2}, {-3, 2},
	};
	index %= sizeof(formations) / sizeof(formations[0]);
	return formations[index];
}

bool unit::isbusy() const {
	return action_time > game.time;
}

bool unit::ismoving() const {
	return screen != m2sc(position);
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

void unit::blockland() const {
	// 1) Prepare path map and block impassable landscape
	area.blockland(geti().move);
	// 2) Block all tiles with units except this one
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

direction unit::nextpath(point v) {
	blockland();
	if(path_map[v.y][v.x] == BlockArea)
		return Center;
	else {
		area.makewave(order, geti().move); // Consume time action
		return area.moveto(position, move_direction);
	}
}

static point next_screen(point v, direction d) {
	// Center, Up, RightUp, Right, RightDown, Down, LeftDown, Left, LeftUp,
	static point movesteps[LeftUp + 1] = {
		{0, 0}, {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1},
	};
	return v + movesteps[d];
}

void unit::movescreen() {
	screen = next_screen(screen, move_direction);
	auto move_speed = getspeed();
	start_time += getspeed();
	if(isdiagonal(move_direction))
		start_time += move_speed / 3;
}

static int get_trail_param(direction d) {
	switch(d) {
	case RightUp: case LeftDown: return 1;
	case Left: case Right: return 2;
	case LeftUp: case RightDown: return 3;
	default: return 0;
	}
}

void unit::leavetrail() {
	auto previous_position = s2m(screen);
	if(previous_position != position) {
		previous_position = to(position, to(move_direction, Down));
		if(area.issand(previous_position)) {
			if(isturret())
				area.set(previous_position, Trail, get_trail_param(move_direction));
			else
				area.set(previous_position, Trail, 4 + get_trail_param(move_direction));
		}
	}
}

void unit::update() {
	if(ismoving()) {// Unit just moving to neightboar tile. Must finish.
		movescreen();
		leavetrail();
		if(!isbusy()) { // If not busy we can make some actions when moving
			if(isturret() && shoot_direction != move_direction) {
				shoot_direction = to(shoot_direction, turnto(shoot_direction, move_direction));
				wait(1000);
			}
		}
		if(!ismoving())
			path_direction = Center; // Arrive to next tile, we need new path direction.
	} else if(ismoveorder()) {
		// We ready to start movement to next tile.
		if(path_direction == Center)
			path_direction = nextpath(order);
		if(path_direction == Center) {
			if(game_chance(30))
				stop();
			else
				start_time += game_rand(200, 300);
		} else {
			if(move_direction != path_direction) // One turn is free
				move_direction = to(move_direction, turnto(move_direction, path_direction));
			if(move_direction != path_direction) // More that one turn take time
				start_time += game_rand(300, 400);
			else {
				position = to(position, move_direction); // Mark of next tile as busy. It's impotant.
				movescreen();
			}
		}
	} else {
		if(isturret()) { // Turret random look around
			auto turn_direction = turnto(shoot_direction, move_direction);
			if(turn_direction != Center && game_chance(50))
				shoot_direction = to(shoot_direction, turn_direction);
			else if(game_chance(30))
				shoot_direction = to(shoot_direction, (game_rand() % 2) ? Left : Right);
		}
	}
}

void unit::move(point v) {
	if(!area.isvalid(v))
		return;
	order = v;
	start_time = game.time; // Can't wait command
}

void unit::stop() {
	path_direction = Center;
	order = position;
}

void unit::wait(unsigned long n) {
	if(action_time < game.time)
		action_time = game.time;
	action_time += n;
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

unit* find_unit(point v) {
	for(auto& e : bsdata<unit>()) {
		if(e && e.position == v)
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
	last_unit->shoot_direction = d;
	last_unit->path_direction = Center;
	last_unit->hits = last_unit->getmaximum(Hits);
	last_unit->supply = last_unit->getmaximum(Supply);
	last_unit->setplayer(player);
}

void unit::apply(ordern type, point v) {
	auto opponent = find_unit(v);
	if(opponent)
		return;
	switch(type) {
	case Stop: stop(); break;
	case Move: move(v); break;
	case Retreat: getplayer()->add(Credits, 20); break;
	}
}