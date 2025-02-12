#pragma once

#include "direction.h"
#include "drawable.h"
#include "player.h"
#include "resid.h"

enum unitn : unsigned char {
	LightInfantry, HeavyInfantry, Trike, Tank, AssaultTank
};
enum statn : unsigned char {
	Hits, Damage, Attacks, Speed, Supply, Armor,
};
enum squadn : unsigned char;
enum movementn : unsigned char;
struct stati : nameable {
};
struct uniti : nameable {
	movementn		move;
	resid			res;
	unsigned char	frame, frame_shoot, frame_avatar;
	unsigned char	stats[Armor + 1];
};
struct unit : drawable {
	explicit operator bool() const { return hits > 0; }
	point			position, order;
	unitn			type;
	squadn			squad;
	direction		move_direction, shoot_direction, path_direction;
	unsigned char	player;
	short			hits, supply;
	unsigned long	action_time; // Start animation
	//void			attack();
	void			blockunits() const;
	const uniti&	geti() const;
	playeri*		getplayer() const;
	int				get(statn v) const { return geti().stats[v]; }
	int				getmaximum(statn v) const;
	const char*		getname() const { return geti().getname(); }
	int				getspeed() const;
	bool			ismoveorder() const { return position != order; }
	bool			isturret() const { return geti().frame_shoot != 0; }
	void			move(point v);
	//void			reatreat();
	void			set(point v);
	void			setplayer(const playeri* v);
	void			stop();
	void			update();
};
extern unit *last_unit, *spot_unit;

void add_unit(point pt, direction d, unitn id, const playeri* player);
unit* find_unit(point s);