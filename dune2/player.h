#pragma once

#include "fraction.h"
#include "nameable.h"

const int player_maximum = 6; // 0 - neutral and 1-5 for player humans/ai.

enum abilityn : unsigned char {
	Credits, Energy, Supply
};
struct abilityi : nameable {
};
struct playeri : fractionable {
	unsigned char	color_index;
	unsigned int	abilities[Supply + 1];
	unsigned int	maximum[Supply + 1];
	unsigned short	buildings[32]; // Count of all buildings by type
	unsigned short	units[32]; // Count of all units by type
	void			add(abilityn v, int i) { abilities[v] += i; }
	void			clear();
	unsigned int	get(abilityn v) const { return abilities[v]; }
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