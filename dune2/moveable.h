#pragma once

#include "actable.h"
#include "drawable.h"
#include "movement.h"

struct moveable : drawable, actable {
	point			order, guard;
	direction		move_direction, path_direction;
	explicit operator bool() const { return hits > 0; }
	bool			closing(int action_range);
	bool			isboard() const { return position.x < 0; }
	bool			ismoving() const;
	bool			moving(movementn movement, int move_speed, int line_of_sight);
	bool			nextmoving(movementn movement, int move_speed);
	void			startmove(int move_speed);
	void			stop();
	void			stopmove();
private:
	void			leavetrail(bool heavy_trail);
	void			movescreen(int move_speed);
	direction		nextpath(point v, movementn movement);
	void			scouting(int line_of_sight);
};

void blockland(movementn movement);
void blockunits();
void blockunits(unsigned char player);
void blockunits(unsigned char player, movementn move);