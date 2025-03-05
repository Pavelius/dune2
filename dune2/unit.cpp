#include "area.h"
#include "bsdata.h"
#include "building.h"
#include "fix.h"
#include "game.h"
#include "math.h"
#include "movement.h"
#include "print.h"
#include "player.h"
#include "pushvalue.h"
#include "stringbuilder.h"
#include "squad.h"
#include "unit.h"
#include "unita.h"
#include "view_map.h"

BSDATAC(unit, 2048)

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
	cleanup();
	if(area.isvalid(position)) {
		fixstate("UnitDestroyed");
		switch(getmove(type)) {
		case Wheeled: add_effect(m2sc(position), FixBikeExplosion); break;
		case Tracked: add_effect(m2sc(position), FixExplosion); break;
		case Footed: area.set(position, Bodies); break;
		default: break;
		}
	}
	clear();
}

void unit::damage(int value) {
	value -= get(Armor);
	if(value <= 0) {
		if(game_chance(50))
			value = 1;
	}
	if(value <= 0)
		return;
	if(value >= hits) {
		destroy();
		return;
	}
	hits -= value;
	switch(getmove(type)) {
	case Tracked:
	case Wheeled:
		if(hits <= gethitsmax() / 2)
			set(Smoke);
		break;
	}
}

bool unit::devour() {
	auto move = getmove(type);
	if(move == Undersand) {
		auto pu = find_unit(position, this);
		if(!pu)
			return false;
		pu->fixstate("UnitDevoured");
		pu->cleanup();
		pu->clear();
		add_effect(screen, WormDevour);
		start_time += 3 * 1000;
		return true;
	} else if(move == Tracked) {
		auto pu = find_unit(position, this);
		if(!pu || getmove(pu->type) != Footed)
			return false;
		area.set(pu->position, Blood);
		pu->destroy();
		return true;
	}
	return false;
}

short unsigned unit::getindex() const {
	return this - bsdata<unit>::elements;
}

void unit::setposition(point v) {
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
	if(bsdata<playeri>::elements[player].scout == i)
		bsdata<playeri>::elements[player].scout = 0xFFFF;
	if(last_unit == this)
		last_unit = 0;
	human_selected.remove(this);
}

void unit::scouting() {
	if(position.x < 0)
		return;
	area.scouting(position, player, getlos());
}

void blockunits(unsigned char player, movementn move) {
	for(auto& e : bsdata<unit>()) {
		if(!e || e.isboard())
			continue;
		if(e.player != player && getmove(e.type) == move)
			continue;
		path_map[e.position.y][e.position.x] = BlockArea;
	}
}

void blockunits(unsigned char player) {
	for(auto& e : bsdata<unit>()) {
		if(!e || e.isboard())
			continue;
		if(e.player != player)
			continue;
		path_map[e.position.y][e.position.x] = BlockArea;
	}
}

void blockunits() {
	for(auto& e : bsdata<unit>()) {
		if(e && !e.isboard())
			path_map[e.position.y][e.position.x] = BlockArea;
	}
}

int unit::getspeed() const {
	auto n = get(Speed);
	if(getmove(type) == Wheeled) {
		auto t = area.get(position);
		if(t == Rock)
			n /= 2;
	}
	return getspeedfp(n);
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
	return getpurpose() == Harvest && action > 0 && is(NoEffect);
}

bool unit::istrallfull() const {
	return getpurpose() == Harvest && action >= 10;
}

bool unit::releasetile() {
	if(isboard())
		return false; // Two harvester is unboard and have same position!!! Cut off this case.
	auto p = find_unit(position, this);
	if(!p)
		return false;
	auto move = getmove(type);
	auto enemy_move = getmove(p->type);
	if(enemy_move == Tracked && move == Footed)
		return false;
	if(enemy_move == Undersand)
		return false;
	for(auto d : all_directions) {
		auto v = to(position, d);
		if(area.isblocked(v, move))
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
	if(type != Harvester)
		return false;
	remove(NoEffect);
	if(isboard()) {
		auto pb = find_board(this);
		if(!pb || pb->type != Refinery)
			return false;
		if(action) {
			if(!pb->getplayer().canafford(Credits, 100)) {
				fixstate("ReachSpiceMaximum");
				start_time += 5 * 1000;
				return true;
			}
			action--;
			pb->getplayer().add(Credits, 100);
			start_time += 1000;
		} else {
			pb->unboard();
			if(area.isvalid(target_position)) {
				// Return to work
				order = target_position;
				start_time = game.time;
			}
		}
		return true;
	}
	if(istrallfull()) {
		retreat();
		return true;
	}
	if(!area.isvalid(target_position))
		return false;
	auto v = area.nearest(position, isspicefield, getlos() + 2);
	if(!area.isvalid(v)) { 
		if(position != target_position) {
			order = target_position;
			return true;
		} else {
			stop();
			return false;
		}
	}
	if(position != v)
		order = v;
	else {
		set(NoEffect);
		action++;
		start_time += 1000 * 4;
		switch(area.get(position)) {
		case Spice: area.set(position, Sand); break;
		case SpiceRich: area.set(position, Spice); action++; break;
		}
		fixstate("HarvesterWork");
	}
	return true;
}

unit* find_enemy(point v, unsigned char player, int range) {
	const auto range_multiplier = 10;
	unit* result = 0;
	int result_priority = 1000 * range_multiplier;
	for(auto& e : bsdata<unit>()) {
		if(!e || e.isboard() || e.player == player)
			continue;
		if(e.position.range(v) > range || !area.is(e.position, player, Visible))
			continue;
		auto priority = v.range(e.position) * range_multiplier + e.hits / 10;
		if(!result || priority < result_priority) {
			result = &e;
			result_priority = priority;
		}
	}
	return result;
}

unit* find_enemy(point v, unsigned char player, int range, movementn move) {
	const auto range_multiplier = 10;
	unit* result = 0;
	int result_priority = 1000 * range_multiplier;
	for(auto& e : bsdata<unit>()) {
		if(!e || e.isboard() || e.player == player || getmove(e.type) != move)
			continue;
		if(e.position.range(v) > range || !area.is(e.position, player, Visible))
			continue;
		auto priority = v.range(e.position) * range_multiplier + (10 - e.hits / 10);
		if(!result || priority < result_priority) {
			result = &e;
			result_priority = priority;
		}
	}
	return result;
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

void add_unit(point pt, objectn id, direction d, unsigned char player) {
	pt = area.nearest(pt, isfreetrack, 4);
	if(!area.isvalid(pt))
		return;
	last_unit = bsdata<unit>::addz();
	last_unit->render = last_unit->renderindex();
	last_unit->screen = m2sc(pt);
	last_unit->position = pt;
	last_unit->type = id;
	last_unit->squad = NoSquad;
	last_unit->move_direction = d;
	last_unit->shoot_direction = d;
	last_unit->hits = last_unit->gethitsmax();
	last_unit->player = player;
	last_unit->start_time = game.time;
	last_unit->stop();
	last_unit->scouting();
}

void unit::clear() {
	memset(this, 0, sizeof(*this));
}

void unit::cantdothis() {
	start_time += game_rand(4, 8) * 1000;
	fixstate("UnitCantDoThis");
}

static building* get_main_base(unsigned char player) {
	auto pb = find_base(ConstructionYard, player);
	if(!pb)
		pb = find_base(HeavyVehicleFactory, player);
	if(!pb)
		pb = find_base(RadarOutpost, player);
	return pb;
}

static building* get_refinery(unsigned char player) {
	return find_base(Refinery, player);
}

bool unit::retreat() {
	auto kind = getpurpose();
	if(kind == Harvest) {
		auto pb = get_refinery(player);
		if(!pb) {
			cantdothis(); // Something wrong
			return false; // Not any valid base present
		}
		auto v = area.nearest(position, pb->position, getmove(type), 24, player);
		if(!area.isvalid(v)) {
			cantdothis(); // Something wrong
			return false;
		}
		if(position == v) {
			fixstate("HarvesterUnload");
			pb->board(this);
		} else {
			fixstate("HarvesterReturn");
			order = v;
		}
		return true;
	} else {
		auto pb = get_main_base(player);
		if(!pb) {
			cantdothis(); // Something wrong
			stop();
			return false; // Not any valid base present
		}
		auto v = area.nearest(position, pb->position, getmove(type), 24, player);
		if(!area.isvalid(v)) {
			cantdothis(); // Something wrong - path is blocking
			return false;
		}
		if(position == v)
			stop(); // Just arrived to final place
		else
			order = v;
	}
	return false;
}

void unit::setorder(ordern type, point v) {
	switch(type) {
	case Attack: setaction(type, v, true); break;
	case Harvest: setaction(type, v, false); order = v; break;
	case Move: stop(); order = v; start_time = game.time; break;
	case Retreat: retreat(); break;
	default: stop(); break;
	}
}

void unit::setorder(point v) {
	auto opponent = find_unit(v);
	auto purpose = getpurpose();
	if(purpose == Attack && opponent && opponent->isenemy(player))
		setorder(Attack, v);
	else if(purpose == Harvest && isspicefield(v))
		setorder(Harvest, v);
	else if(area.isvalid(v))
		setorder(Move, v);
}

void unit::recovery() {
	for(auto i = Smoke; i <= BurningFire; i = (fixn)(i + 1)) {
		if(is(i) && game_chance(20))
			remove(i);
	}
}

bool unit::relax() {
	if(game_chance(30)) { // 30% chance do nothing
		if(isturret()) { // Rotate turret
			auto turn_direction = turnto(shoot_direction, move_direction);
			if(turn_direction != Center && game_chance(50))
				shoot_direction = to(shoot_direction, turn_direction);
			else if(game_chance(30))
				shoot_direction = to(shoot_direction, (game_rand() % 2) ? Left : Right);
			return true;
		}
	}
	return false;
}

bool unit::usecrushing() {
	auto move = getmove(type);
	if(move == Tracked || move == Undersand) {
		unit* p = 0;
		if(move == Tracked)
			p = find_enemy(position, player, getlos(), Footed);
		else
			p = find_enemy(position, player, getlos());
		if(p && !area.isblocked(p->position, move)) {
			order = p->position;
			return true;
		}
	}
	return false;
}

bool unit::shoot() {
	return actable::shoot(screen, getweapon(type), get(Attacks), getshootrange());
}

void unit::update() {
	auto move = getmove(type);
	auto speed = getspeed();
	if(moving(move, speed, getlos())) {
		if(!ismoving() && devour())
			return;
		if(!isturret())
			shoot_direction = move_direction;
		else if(shoot())
			return;
		else if(!area.isvalid(target_position))
			turn(shoot_direction, move_direction);
		return;
	} else if(launch(screen, getheavyweapon(type), 6))
		return;
	else if(shoot()) {
		if(!isturret())
			move_direction = shoot_direction;
		stopmove();
		return;
	} else if(nextmoving(move, speed))
		return;
	else if(usecrushing())
		return;
	else if(releasetile())
		return;
	else if(harvest())
		return;
	else if(seeking(getshootrange()))
		return;
	else if(closing())
		return;
}