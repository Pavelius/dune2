#include "bsdata.h"
#include "squad.h"
#include "unit.h"

bool squad_active(squadn index, const playeri* player) {
	auto player_index = player->getindex();
	for(auto& e : bsdata<unit>()) {
		if(e && e.squad == index && e.player == player_index)
			return true;
	}
	return false;
}

bool squad_select(squadn index, const playeri* player, const rect& screen_area) {
	auto result = false;
	auto player_index = player->getindex();
	for(auto& e : bsdata<unit>()) {
		if(!e || e.player != player_index)
			continue;
		if(e.screen.in(screen_area)) {
			e.squad = index;
			result = true;
		} else if(e.squad == index)
			e.squad = NoSquad;
	}
	return result;
}