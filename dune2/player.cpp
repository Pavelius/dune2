#include "bsdata.h"
#include "player.h"

BSDATAC(playeri, 8)

playeri* player;

unsigned char playeri::getindex() const {
	if(!this)
		return 0xFF;
	return this - bsdata<playeri>::elements;
}

playeri* playerable::getplayer() const {
	if(player == 0xFF)
		return 0;
	return bsdata<playeri>::elements + player;
}

void playerable::setplayer(const playeri* v) {
	if(v)
		player = v - bsdata<playeri>::elements;
	else
		player = 0xFF;
}