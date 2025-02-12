#include "unita.h"

unita human_selected;

void unita::select(const playeri* player, const rect& screen_area) {
	auto player_index = player->getindex();
	for(auto& e : bsdata<unit>()) {
		if(!e || e.player != player_index)
			continue;
		if(e.screen.in(screen_area))
			add(&e);
	}
}