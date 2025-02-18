#include "area.h"
#include "bsdata.h"
#include "fix.h"
#include "game.h"
#include "math.h"
#include "movement.h"
#include "squad.h"
#include "unit.h"
#include "view.h"

///////////////////////////////////////////////////////////
// SOME DESCRIPTION
///////////////////////////////////////////////////////////
// Units with turret have advantage. They can move and shoot.
// Bikes can move, but must stop and turn to shoot directions.
// Damage mechanic is next. Each unit have Damage rating. Value between 3-10.

BSDATAC(unit, 2048)
BSDATA(uniti) = {
	{"LightInfantry", INFANTRY, 81, 40, Footed, ShootHandGun, UNITS, 91, 0, {4, 2, 1, 3}},
	{"HeavyInfantry", HYINFY, 91, 70, Footed, ShootHandGun, UNITS, 103, 0, {4, 3, 1, 2, 1}},
	{"Trike", TRIKE, 80, 0, Wheeled, ShootHandGun, UNITS, 5, 0, {6, 4, 2, 10, 2}},
	{"Tank", LTANK, 78, 0, Tracked, ShootHeavyGun, UNITS2, 0, 5, {8, 6, 1, 5, 3}},
	{"AssaultTank", HTANK, 72, 0, Tracked, ShootHeavyGun, UNITS2, 10, 15, {9, 6, 2, 4, 4}},
};
assert_enum(uniti, AssaultTank)

unit *last_unit;

const unsigned long shoot_duration = 1000 * 3;
const unsigned long shoot_next_attack = 200;
const unsigned long look_duration = 500;

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

void unit::damage(int value) {
	if(hits > value) {
		hits -= value;
		return;
	}
	cleanup();
	switch(geti().move) {
	case Wheeled:
		add_effect(m2sc(position), FixBikeExplosion, 0xFFFF);
		break;
	case Tracked:
		add_effect(m2sc(position), FixExplosion, 0xFFFF);
		break;
	}
	clear();
}

bool unit::isbusy() const {
	return shoot_time > game.time;
}

bool unit::isenemy() const {
	return player == ::player->getindex();
}

bool unit::ismoving() const {
	return screen != m2sc(position);
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
			e.stopattack();
	}
	for(auto& e : bsdata<draweffect>()) {
		if(!e)
			continue;
		if(e.owner == i)
			e.owner = 0xFFFF;
	}
}

void unit::scouting() {
	area.scouting(position, player, getlos());
}

void blockunits(const unit* exclude) {
	for(auto& e : bsdata<unit>()) {
		if(e && &e != exclude)
			path_map[e.position.y][e.position.x] = BlockArea;
	}
}

void unit::blockland() const {
	area.blockland(geti().move);
	blockunits(this);
}

int unit::getspeed() const {
	auto n = get(Speed);
	if(!n)
		return 64 * 4 * 2;
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

bool unit::isattacking() const {
	return area.isvalid(order_attack);
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

void unit::tracking() {
	auto enemy = getenemy();
	if(enemy)
		order_attack = enemy->position;
}

void unit::stopattack() {
	target = 0xFFFF;
	order_attack = {-10000, -10000};
}

bool unit::canshoot() const {
	if(!area.isvalid(order_attack))
		return false;
	if(!area.is(order_attack, player, Visible))
		return false;
	auto range = position.range(order_attack);
	return range <= getshootrange();
}

void unit::fixshoot() {
	auto weapon = geti().weapon;
	add_effect(screen, m2sc(order_attack), weapon, getindex());
}

bool unit::shoot() {
	if(shoot_time > game.time) {
		if(attacks) {// Allow multi-attacks
			if((shoot_time - game.time) >= (attacks * shoot_next_attack)) {
				fixshoot(); // Can make next attack on same target
				attacks++;
				if(attacks >= geti().stats[Attack])
					attacks = 0;
			}
		}
		return true;
	}
	if(!canshoot())
		stopattack();
	else {
		auto d = to(position, order_attack);
		if(isturret()) {
			if(!turn(shoot_direction, d)) {
				wait(look_duration);
				return true;
			}
		} else {
			if(!turn(move_direction, d)) {
				wait(look_duration);
				return true;
			}
		}
		fixshoot();
		wait(shoot_duration);
		if(get(Attacks) > 1)
			attacks = 1;
		else
			attacks = 0;
		return true;
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
			} else if(!isbusy()) {// If not busy we can make some actions while moving
				if(shoot_direction != move_direction) {
					shoot_direction = to(shoot_direction, turnto(shoot_direction, move_direction));
					wait(look_duration);
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
				start_time += game_rand(200, 300);
		} else if(geti().move == Footed) {
			move_direction = path_direction; // Footed units turn around momentary.
			position = to(position, move_direction); // Mark of next tile as busy. It's impotant.
			movescreen(); // Start moving to next tile.
		} else {
			if(!turn(move_direction, path_direction)) // More that one turn take time
				start_time += game_rand(300, 400); // Turning pause
			else {
				position = to(position, move_direction); // Mark of next tile as busy. It's impotant.
				movescreen(); // Start moving to next tile.
			}
		}
	} else if(shoot()) {
		// After shoot: do nothing now
	} else if(isturret()) { // Turret random look around
		auto turn_direction = turnto(shoot_direction, move_direction);
		if(turn_direction != Center && game_chance(50))
			shoot_direction = to(shoot_direction, turn_direction);
		else if(game_chance(30))
			shoot_direction = to(shoot_direction, (game_rand() % 2) ? Left : Right);
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
	stopattack();
}

void unit::wait(unsigned long n) {
	if(shoot_time < game.time)
		shoot_time = game.time;
	shoot_time += n;
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
	last_unit->order_attack = {-10000, -10000};
	last_unit->type = id;
	last_unit->squad = NoSquad;
	last_unit->move_direction = d;
	last_unit->shoot_direction = d;
	last_unit->path_direction = Center;
	last_unit->hits = last_unit->getmaximum(Hits);
	last_unit->target = 0xFFFF;
	last_unit->setplayer(player);
	last_unit->scouting();
}

void unit::clear() {
	memset(this, 0, sizeof(*this));
}

void unit::apply(ordern type, point v) {
	auto opponent = find_unit(v);
	switch(type) {
	case Attack:
		if(opponent == this)
			break; // Big mistake
		if(opponent) {
			target = opponent->getindex();
			order_attack = opponent->position;
		} else {
			target = 0xFFFF;
			order_attack = v;
		}
		break;
	case Move:
		if(opponent && opponent->isenemy())
			apply(Attack, v);
		else
			move(v);
		break;
	default:
		stop();
		break;
	}
}

unit* unit::getenemy() const {
	if(target == 0xFFFF)
		return 0;
	return bsdata<unit>::elements + target;
}