#pragma once

#include "ability.h"
#include "fraction.h"

const int player_maximum = 6; // 0 - neutral and 1-5 for player humans/ai.

struct playeri : fractionable, abilityable {
	unsigned char	color_index;
	unsigned short	buildings[32]; // Count of all buildings by type
	unsigned short	units[32]; // Count of all units by type
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