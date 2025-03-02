#pragma once

#include "fraction.h"
#include "point.h"
#include "storeable.h"

const int player_maximum = 6; // 0 - neutral and 1-5 for player humans/ai.

struct playeri : fractionable, storeable {
	unsigned char	color_index;
	unsigned char	region;
	unsigned short	objects[64]; // Count of all by type
	point			base, spice, enemy, enemy_base;
	unsigned long	enemy_spot_turn;
	unsigned short	scout;
	bool			cansupportenergy(objectn type) const;
	void			clear();
	unsigned char	getindex() const;
	void			update();
};
struct playerable {
	unsigned char	player;
	playeri&		getplayer() const;
	void			setplayer(const playeri* v);
};
extern unsigned char player_index, player_human;

playeri& mainplayer();