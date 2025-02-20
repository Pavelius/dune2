#include "actable.h"
#include "area.h"
#include "bsdata.h"
#include "direction.h"
#include "game.h"
#include "player.h"
#include "print.h"
#include "stringbuilder.h"
#include "unit.h"

const unsigned long action_next_time = 200;
const unsigned long action_duration = 3 * 1000;

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
	if(!area.is(position, ::player->getindex(), Visible))
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

bool actable::shoot(point screen, fixn weapon, int attacks, int maximum_range) {
	if(!weapon)
		return false;
	if(action_time > game.time) {
		if(action) {// Allow multi-attacks
			if((action_time - game.time) >= (action * action_next_time)) {
				fixshoot(screen, m2sc(target_position), weapon, 40); // Can make next attack on same target, but can miss
				action++;
				if(action >= attacks)
					action = 0;
			}
		}
		return true;
	} else if(!canshoot(maximum_range))
		stop();
	else {
		auto d = to(position, target_position);
		if(!turn(action_direction, d)) {
			wait(look_duration);
			return true;
		}
		fixshoot(screen, m2sc(target_position), weapon, 0);
		wait(action_duration);
		if(attacks > 1)
			action = 1;
		else
			action = 0;
		return true;
	}
	return false;
}

void actable::stop() {
	target = 0xFFFF;
	target_position = {-10000, -10000};
}

void actable::wait(int duration) {
	if(action_time < game.time)
		action_time = game.time;
	action_time += duration;
}

bool actable::isready() const {
	return action_time <= game.time;
}

bool actable::isenemy(unsigned char player_index) const {
	return player != player_index;
}

void actable::setaction(point v, bool hostile) {
	auto p = find_unit(v);
	if(p && p->isenemy(player) == hostile)
		target = p->getindex();
	else
		target = 0xFFFF;
	target_position = v;
}