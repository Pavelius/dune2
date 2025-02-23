#include "actable.h"
#include "area.h"
#include "bsdata.h"
#include "direction.h"
#include "game.h"
#include "player.h"
#include "print.h"
#include "stringbuilder.h"
#include "unit.h"

bool turn(direction& result, direction new_direction) {
	if(new_direction == Center)
		return false;
	if(result != new_direction)
		result = to(result, turnto(result, new_direction));
	return result == new_direction;
}

point random_near(point v) {
	return to(v, all_directions[game_rand() % (sizeof(all_directions) / sizeof(all_directions[0]))]);
}

const char* actable::getfractionname() const {
	return getplayer().getfraction().getname();
}

void actable::fixstate(const char* id) const {
	if(isboard()) {
		if(player != player_index)
			return;
	} else if(!area.is(position, player_index, Visible))
		return;
	print(getnm(id));
}

unit* actable::getenemy() const {
	if(target == 0xFFFF)
		return 0;
	return bsdata<unit>::elements + target;
}

void actable::tracking() {
	auto enemy = getenemy();
	if(enemy)
		target_position = enemy->position;
}

void actable::fixshoot(point screen, point to, fixn weapon, int chance_miss) {
	if(chance_miss && game_chance(chance_miss)) {
		auto miss = random_near(s2m(to));
		auto current = s2m(screen);
		if(miss == current)
			return; // Hit itself?
		add_effect(screen, m2sc(miss), weapon);
	} else
		add_effect(screen, to, weapon);
}

bool actable::canshoot(int maximum_range) const {
	if(!area.isvalid(target_position))
		return false;
	if(!area.is(target_position, player, Visible))
		return false;
	auto range = position.range(target_position);
	return range <= maximum_range;
}

static unsigned get_duration(fixn weapon) {
	switch(weapon) {
	case FireRocket: return action_duration + action_duration / 2;
	default: return action_duration;
	}
}

static unsigned get_miss_chance(fixn weapon) {
	switch(weapon) {
	case FireRocket: return 40;
	default: return 0;
	}
}

void actable::shooting(point screen, fixn weapon, int attacks) {
	if(!weapon || !area.isvalid(target_position))
		return;
	auto duration = shoot_time + get_duration(weapon);
	if(shoot_time + action_duration <= game.time) {
		shoot_time = 0;
		return;
	} else if(!action || action >= attacks)
		return;
	if(shoot_time + action * 300 <= game.time) {
		fixshoot(screen, m2sc(target_position), weapon, 40); // Can make next attack on same target, but can miss
		action++;
	}
}

bool actable::shoot(point screen, fixn weapon, int attacks, int maximum_range) {
	if(!weapon || shoot_time)
		return false;
	if(canshoot(maximum_range)) {
		auto d = to(position, target_position);
		if(turn(shoot_direction, d)) {
			fixshoot(screen, m2sc(target_position), weapon, get_miss_chance(weapon));
			action = 1;
			shoot_time = game.time;
		}
		return true;
	}
	return false;
}

bool actable::seeking(int range) {
	auto enemy = getenemy();
	if(!enemy) {
		enemy = find_enemy(position, player, range);
		if(enemy) {
			target = enemy->getindex();
			target_position = enemy->position;
			return true;
		}
	}
	return false;
}

void actable::stop() {
	target = 0xFFFF;
	target_position = {-10000, -10000};
}

bool actable::isenemy(unsigned char player_index) const {
	return player != player_index;
}

void actable::setaction(ordern type, point v, bool lock_unit) {
	if(lock_unit) {
		auto p = find_unit(v);
		target = p ? p->getindex() : 0xFFFF;
	} else
		target = 0xFFFF;
	target_position = v;
}

void actable::unblock() const {
	if(area.isvalid(position))
		path_map[position.y][position.x] = 0;
}