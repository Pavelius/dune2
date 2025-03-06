#pragma once

#include "moveable.h"
#include "object.h"

struct unit;

struct aircraft : moveable, objectable {
	short unsigned cargo;
	explicit operator bool() const { return type != NoObject; }
	void		board(unit* p);
	void		clear();
	void		destroy();
	int			getindex() const;
	bool		iscargo() const { return cargo != 0xFFFF; }
	void		leave();
	void		returnbase();
	void		unboard();
	void		update();
private:
	void		cleanup(bool destroying);
	void		patrol();
};
extern aircraft* last_aircraft;

void add_air_unit(point pt, objectn id, direction d, unsigned char player);

point get_star_base(point pt, int seed);
point get_star_base(unsigned char player);