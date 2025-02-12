#include "bsdata.h"
#include "player.h"

BSDATAC(playeri, 8)

playeri* player;

unsigned char playeri::getindex() const {
	if(!this)
		return 0xFF;
	return this - bsdata<playeri>::elements;
}