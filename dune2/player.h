#pragma once

#include "fraction.h"
#include "point.h"

const int player_maximum = 6; // 0 - neutral and 1-5 for player humans/ai.

enum statn : unsigned char {
	Credits, Energy,
	LastStat = Energy,
};
extern const char* stat_names[LastStat + 1];

enum ordern : unsigned char {
	Stop, Move, MoveAndAttack, Attack, Retreat, Harvest,
	LastOrder = Harvest
};
extern const char* order_names[LastOrder + 1];

struct storeable {
	unsigned int abilities[LastStat + 1];
	unsigned int maximum[LastStat + 1];
	void add(statn v, int i) { abilities[v] += i; }
	bool canafford(statn v, unsigned int i) const { return abilities[v] + i <= maximum[v]; }
	unsigned int get(statn v) const { return abilities[v]; }
	unsigned int getmaximum(statn v) const { return maximum[v]; }
};

struct playeri : fractionable, storeable {
	unsigned char	color_index;
	unsigned char	region;
	unsigned short	objects[64]; // Count of all by type
	point			base, spice, enemy, enemy_base;
	unsigned long	enemy_spot_turn;
	unsigned short	scout;
	int				enemy_units_spotted, enemy_buildings_spotted;
	unsigned		heavy_shoot_time;
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

inline const char* getnm(statn n) { return stat_names[n]; }
inline const char* getnm(ordern n) { return order_names[n]; }

playeri& mainplayer();