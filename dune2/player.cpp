#include "bsdata.h"
#include "player.h"

BSDATAC(playeri, player_maximum)

playeri* player;

unsigned char playeri::getindex() const {
	if(!this)
		return 0xFF;
	return this - bsdata<playeri>::elements;
}

playeri& playerable::getplayer() const {
	return bsdata<playeri>::elements[player];
}

void playerable::setplayer(const playeri* v) {
	if(v)
		player = v - bsdata<playeri>::elements;
}