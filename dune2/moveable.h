#pragma once

#include "actable.h"
#include "drawable.h"
#include "movement.h"

struct moveable : drawable, actable {
	point			order, guard;
	direction		move_direction, path_direction;
	bool			isboard() const { return position.x < 0; }
	bool			ismoveorder() const { return position != order; }
	bool			moving(movementn movement, int move_speed, int line_of_sight);
	void			startmove(int move_speed);
	void			unblock() const;
private:
	bool			ismoving() const;
	void			leavetrail(bool heavy_trail);
	void			movescreen(int move_speed);
	direction		nextpath(point v, movementn movement);
	void			scouting(int line_of_sight);
};

void blockland(movementn movement);
void blockunits();