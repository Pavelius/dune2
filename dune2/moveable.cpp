#include "area.h"
#include "bsdata.h"
#include "direction.h"
#include "game.h"
#include "moveable.h"
#include "unit.h"

static point next_screen(point v, direction d) {
	// Center, Up, RightUp, Right, RightDown, Down, LeftDown, Left, LeftUp,
	static point movesteps[LeftUp + 1] = {
		{0, 0}, {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1},
	};
	return v + movesteps[d];
}

static int get_trail_param(direction d) {
	switch(d) {
	case RightUp: case LeftDown: return 1;
	case Left: case Right: return 2;
	case LeftUp: case RightDown: return 3;
	default: return 0;
	}
}

static void blockunits(const moveable* exclude) {
	for(auto& e : bsdata<unit>()) {
		if(e /*&& static_cast<moveable*>(&e) != exclude*/ && e.position.x >= 0)
			path_map[e.position.y][e.position.x] = BlockArea;
	}
}

void moveable::startmove(int move_speed) {
	position = to(position, move_direction); // Mark of next tile as busy. It's impotant.
	movescreen(move_speed); // Start moving to next tile.
}

void moveable::movescreen(int move_speed) {
	screen = next_screen(screen, move_direction);
	start_time += move_speed;
	if(isdiagonal(move_direction))
		start_time += move_speed / 3;
}

void moveable::leavetrail(bool heavy_trail) {
	auto previous_position = s2m(screen);
	if(previous_position != position) {
		previous_position = to(position, to(move_direction, Down));
		if(area.issand(previous_position)) {
			if(heavy_trail)
				area.set(previous_position, Trail, get_trail_param(move_direction));
			else
				area.set(previous_position, Trail, 4 + get_trail_param(move_direction));
		}
	}
}

void moveable::scouting(int line_of_sight) {
	if(position.x < 0 || !line_of_sight)
		return;
	area.scouting(position, player, line_of_sight);
}

bool moveable::ismoving() const {
	return !isboard() && screen != m2sc(position);
}

void moveable::blockland(movementn movement) const {
	area.blockland(movement);
}

direction moveable::nextpath(point v, movementn movement) {
	if(!area.isvalid(v))
		return Center;
	auto need_block_units = v.range(position) < 3;
	blockland(movement);
	if(need_block_units)
		blockunits(this);
	if(path_map[v.y][v.x] == BlockArea)
		return Center;
	else {
		area.movewave(v, movement); // Consume time action
		if(!need_block_units)
			blockunits(this);
		return area.moveto(position, move_direction);
	}
}

bool moveable::moving(movementn movement, int move_speed, int line_of_sight, bool turret, fixn weapon, int attacks, int shoot_range) {
	tracking();
	if(ismoving()) {// Unit just moving to neightboar tile. MUST FINISH!!!
		movescreen(move_speed);
		leavetrail(movement == Tracked);
		if(turret) {
			if(shoot(screen, weapon, attacks, shoot_range)) { // Turret vehicle can shoot on moving
				// After shoot do nothing
			} else if(isready()) { // If not busy we can make some actions while moving
				if(action_direction != move_direction) {
					action_direction = to(action_direction, turnto(action_direction, move_direction));
					wait(look_duration);
				}
			}
		}
		if(!ismoving()) {
			scouting(line_of_sight);
			path_direction = Center; // Arrive to next tile, we need new path direction.
		}
	} else if(ismoveorder()) {
		// We ready to start movement to next tile.
		if(path_direction == Center)
			path_direction = nextpath(order, movement);
		if(path_direction == Center) {
			if(game_chance(20)) // Something in the way. Wait or cancel order?
				stop();
			else
				start_time += game_rand(look_duration / 2, look_duration);
		} else if(movement == Footed) {
			move_direction = path_direction; // Footed units turn around momentary.
			startmove(move_speed);
		} else {
			if(!turn(move_direction, path_direction)) // More that one turn take time
				start_time += look_duration / 2; // Turning pause
			else
				startmove(move_speed);
		}
		if(!turret) {
			if(move_direction != Center)
				action_direction = move_direction;
		}
	} else
		return false;
	return true;
}