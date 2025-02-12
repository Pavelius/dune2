#pragma once

#include "nameable.h"
#include "rect.h"

struct playeri;

enum squadn : unsigned char {
	NoSquad, SelectedUnits, 
};
struct squadi : nameable {
};

bool squad_active(squadn index, const playeri* player);
bool squad_select(squadn index, const playeri* player, const rect& screen_area);