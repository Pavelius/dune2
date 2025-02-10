#pragma once

#include "direction.h"
#include "drawable.h"
#include "player.h"
#include "resid.h"

enum unitn : unsigned char {
	LightInfantry, HeavyInfantry, Trike,
};
enum statn : unsigned char {
	Hits, Damage, Attacks, Speed, Supply, Armor,
};
struct stati : nameable {
};
struct uniti : nameable {
	resid			res;
	unsigned char	frame;
	unsigned char	stats[Armor + 1];
};
struct unit : drawable {
	explicit operator bool() const { return hits > 0; }
	unitn			type;
	unsigned char	player;
	short			hits, supply;
	const uniti&	geti() const;
	playeri*		getplayer() const;
	int				get(statn v) const { return geti().stats[v]; }
	int				getmaximum(statn v) const;
	void			set(direction v) { param = v; }
	void			setplayer(const playeri* v);
};
unit* add_unit(point pt, direction d, unitn id, const playeri* player);