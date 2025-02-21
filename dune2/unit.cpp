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

///////////////////////////////////////////////////////////
// SOME DESCRIPTION
///////////////////////////////////////////////////////////
// Units with turret have advantage. They can move and shoot.
// Bikes can move, but must stop and turn to shoot directions.
// Damage mechanic is next. Each unit have Damage rating. Value between 3-10.

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

void blockunits(const unit* exclude) {
	for(auto& e : bsdata<unit>()) {
		if(e && &e != exclude && e.position.x >= 0)
			path_map[e.position.y][e.position.x] = BlockArea;
	}
}

void unit::blockland() const {
	area.blockland(geti().move);
}

int unit::getspeed() const {
	auto n = get(Speed);
	if(!n)
		return 64 * 4 * 2;
	return 64 * 4 / n;
}

direction unit::nextpath(point v) {
	if(!area.isvalid(v))
		return Center;
	auto need_block_units = v.range(position) < 3;
	blockland();
	if(need_block_units)
		blockunits(this);
	if(path_map[v.y][v.x] == BlockArea)
		return Center;
	else {
		area.movewave(v, geti().move); // Consume time action
		if(!need_block_units)
			blockunits(this);
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
	start_time += move_speed;
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
	return getpurpose() == Harvest && !isready();
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
			startmove();
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
			wait(1000);
			start_time = action_time;
		} else {
			pb->unboard();
			if(area.isvalid(target_position))
				apply(Move, target_position);
		}
		return true;
	}
	if(!area.isvalid(target_position))
		return false;
	if(istrallfull()) {
		returnbase();
		return true;
	}
	auto v = area.nearest(position, isspicefield, 4 + getlos());
	if(!area.isvalid(v))
		return false;
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
	} else
		apply(Move, v);
	return true;
}

void unit::startmove() {
	position = to(position, move_direction); // Mark of next tile as busy. It's impotant.
	movescreen(); // Start moving to next tile.
}

bool unit::seeking() {
	if(isturret()) { // Turret random look around
		auto turn_direction = turnto(action_direction, move_direction);
		if(turn_direction != Center && game_chance(50))
			action_direction = to(action_direction, turn_direction);
		else if(game_chance(30))
			action_direction = to(action_direction, (game_rand() % 2) ? Left : Right);
	}
	return false;
}

void unit::update() {
	tracking();
	if(ismoving()) {// Unit just moving to neightboar tile. MUST FINISH!!!
		movescreen();
		leavetrail();
		if(isturret()) {
			if(shoot()) { // Turret vehicle can shoot on moving
				// After shoot do nothing
			} else if(isready()) { // If not busy we can make some actions while moving
				if(action_direction != move_direction) {
					action_direction = to(action_direction, turnto(action_direction, move_direction));
					wait(look_duration / 2);
				}
			}
		}
		if(!ismoving()) {
			scouting();
			path_direction = Center; // Arrive to next tile, we need new path direction.
		}
	} else if(ismoveorder()) {
		// We ready to start movement to next tile.
		if(path_direction == Center)
			path_direction = nextpath(order);
		if(path_direction == Center) {
			if(game_chance(30)) // Something in the way. Wait or cancel order?
				stop();
			else
				start_time += game_rand(look_duration / 2, look_duration);
		} else if(geti().move == Footed) {
			move_direction = path_direction; // Footed units turn around momentary.
			startmove();
		} else {
			if(!turn(move_direction, path_direction)) // More that one turn take time
				start_time += look_duration / 2; // Turning pause
			else
				startmove();
		}
		if(move_direction != Center) {
			if(!isturret())
				action_direction = move_direction;
		}
	} else if(releasetile())
		return;
	else if(harvest())
		return;
	else if(shoot()) {
		if(!isturret()) {
			if(move_direction != action_direction) {
				move_direction = action_direction;
				synchronize();
			}
		}
		return;
	} else if(seeking())
		return;
}

void unit::stop() {
	path_direction = Center;
	order = position;
	actable::stop();
}

void unit::synchronize() {
	if(action_time > start_time)
		start_time = action_time;
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
		auto pb = find_base(Refinery, player);
		if(!pb) {
			cantdothis(); // Something wrong
			return false; // Not any valid base present
		}
		blockland();
		blockunits(this);
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
		blockland();
		blockunits(this);
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
	auto opponent = find_unit(v);
	switch(type) {
	case Attack:
		setaction(v, true);
		break;
	case Harvest:
		setaction(v, false);
		break;
	case Move:
		if(opponent && opponent->isenemy(player) && getpurpose() == Attack)
			apply(Attack, v);
		else if(getpurpose() == Harvest && isspicefield(v))
			apply(Harvest, v);
		else if(area.isvalid(v)) {
			order = v;
			start_time = game.time; // Can't wait command
		}
		break;
	case Retreat:
		returnbase();
		break;
	default:
		stop();
		break;
	}
}