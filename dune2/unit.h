#pragma once

#include "drawable.h"
#include "nameable.h"
#include "player.h"
#include "typeable.h"

enum statn : unsigned char {
	Hits, Damage, Attacks, Speed, Supply, Armor,
};
struct stati : nameable {
	unsigned char	stats[Armor + 1];
};
struct uniti : nameable {
	unsigned char	stats[Armor + 1];
};
struct unit : drawable, typeable<uniti>, playerable {
	explicit operator bool() const { return hits > 0; }
	short			hits, supply;
};
