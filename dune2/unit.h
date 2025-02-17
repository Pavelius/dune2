#pragma once

#include "direction.h"
#include "drawable.h"
#include "player.h"
#include "order.h"
#include "resid.h"
#include "topicable.h"
#include "typeable.h"

enum unitn : unsigned char {
	LightInfantry, HeavyInfantry, Trike, Tank, AssaultTank
};
enum statn : unsigned char {
	Hits, Damage, Attacks, Speed, Armor,
};
enum squadn : unsigned char;
enum movementn : unsigned char;
struct stati : nameable {
};
struct uniti : topicable {
	movementn		move;
	resid			res;
	unsigned char	frame, frame_shoot;
	unsigned char	stats[Armor + 1];
};
struct unit : drawable, playerable, typeable<uniti, unitn> {
	unsigned long	action_time; // Start action
	point			position, order, guard, order_attack;
	squadn			squad;
	direction		move_direction, shoot_direction, path_direction;
	short unsigned	target; // Enemy target
	short			hits;
	explicit operator bool() const { return hits > 0; }
	void			apply(ordern type, point v);
	void			attack(unit* opponent);
	int				get(statn v) const { return geti().stats[v]; }
	unit*			getenemy() const;
	int				getmaximum(statn v) const;
	short unsigned	getindex() const;
	const char*		getname() const { return geti().getname(); }
	int				getspeed() const;
	bool			isattackorder() const;
	bool			isbusy() const;
	bool			ismoveorder() const { return position != order; }
	bool			ismoving() const;
	bool			isturret() const { return geti().frame_shoot != 0; }
	bool			iswaitorder() const { return !ismoveorder() && !ismoving(); }
	void			move(point v);
	void			scouting();
	void			set(point v);
	void			stop();
	void			update();
	void			wait(unsigned long n);
private:
	void			blockland() const;
	void			leavetrail();
	void			movescreen();
	direction		nextpath(point v);
	void			tracking();
	direction		targetdirection() const;
	void			turnturret(direction d);
};
extern unit *last_unit;

void add_unit(point pt, unitn id, direction d);
bool isnonblocked(point v);
bool isfreetrack(point v);
bool isfreefoot(point v);

unit* find_unit(point s);
point formation(int index);
