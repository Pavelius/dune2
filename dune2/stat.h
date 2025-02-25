#pragma once

#include "nameable.h"

struct stati : nameable {
};
enum statn : unsigned char {
	Credits, Energy, Supply,
	Hits, Attacks, Speed, Armor, Range,
	LoS,
};