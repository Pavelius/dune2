#pragma once

#include "adat.h"
#include "moveable.h"
#include "object.h"

struct aircraft : moveable, objectable {
	adat<short unsigned, 6> load;
	explicit operator bool() const { return type != NoObject; }
	void		clear();
	void		destroy();
	int			getindex() const;
	point		getstarbase(point v) const;
	void		leave();
	void		returnbase();
	void		update();
private:
	void		cleanup(bool destroying);
	void		patrol();
};
void add_air_unit(point pt, objectn id, direction d, unsigned char player);