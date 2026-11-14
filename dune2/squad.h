#pragma once

struct playeri;
struct rect;

enum squadn : unsigned char {
	NoSquad, SelectedUnits, 
};

bool squad_active(squadn index, const playeri* player);
bool squad_select(squadn index, const playeri* player, const rect& screen_area);