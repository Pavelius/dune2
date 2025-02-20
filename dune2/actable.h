#pragma once

#include "player.h"
#include "point.h"

enum direction : unsigned char;
enum fixn : unsigned char;

struct unit;

struct actable : playerable {
	point			position;
	unsigned char	action; // Sequence action in a row
	direction		action_direction;
	unsigned long	action_time; // Start action time
	point			target_position;
	short unsigned	target; // Enemy unit target
	short			hits;
	bool			canshoot(int maximum_range) const;
	unit*			getenemy() const;
	const char*		getfractionname() const;
	void			fixstate(const char* id) const;
	static void		fixshoot(point from, point to, fixn weapon, int chance_miss);
	bool			shoot(point screen, fixn weapon, int attacks, int maximum_range);
	void			stop();
	void			tracking();
	void			wait(int duration);
};

point random_near(point v);
bool turn(direction& result, direction new_direction);