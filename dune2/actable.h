#pragma once

#include "player.h"
#include "order.h"
#include "point.h"

enum direction : unsigned char;
enum fixn : unsigned char;

struct unit;

const unsigned long look_duration = 400;
const unsigned long action_duration = 3 * 1000;

struct actable : playerable {
	point			position, target_position;
	short unsigned	target; // Enemy unit target
	short unsigned	hits;
	unsigned char	action; // Sequence action in a row
	direction		shoot_direction;
	ordern			order_type; // Type of main order
	unsigned long	shoot_time; // Start shoot game time
	explicit operator bool() const { return hits > 0; }
	unit*			getenemy() const;
	const char*		getfractionname() const;
	bool			isboard() const { return position.x < 0; }
	bool			isenemy(unsigned char player_index) const;
	void			fixstate(const char* id) const;
	static void		fixshoot(point from, point to, fixn weapon, int chance_miss);
	void			setaction(ordern type, point v, bool lock_unit);
	bool			shoot(point screen, fixn weapon, int attacks, int maximum_range);
	void			shooting(point screen, fixn weapon, int attacks);
	void			stop();
	void			tracking();
	void			unblock() const;
private:
	bool			canshoot(int maximum_range) const;
};

point random_near(point v);
bool turn(direction& result, direction new_direction);