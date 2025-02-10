#pragma once

#include "nameable.h"

enum abilityn : unsigned char {
	Credits, Rounds,
};
struct abilityi : nameable {
};
struct playeri {
	unsigned	abilities[Rounds + 1];
};
struct playerable {
	short unsigned	player;
};