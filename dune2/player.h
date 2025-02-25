#pragma once

#include "storeable.h"
#include "fraction.h"

const int player_maximum = 6; // 0 - neutral and 1-5 for player humans/ai.

struct playeri : fractionable, storeable {
	unsigned char	color_index;
	unsigned short	objects[64]; // Count of all by type
	void			clear();
	unsigned char	getindex() const;
	void			update();
};
struct playerable {
	unsigned char	player;
	playeri&		getplayer() const;
	void			setplayer(const playeri* v);
};
extern unsigned char player_index;

playeri& mainplayer();