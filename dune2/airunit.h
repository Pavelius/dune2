#pragma once

#include "moveable.h"
#include "object.h"

struct airunit : moveable, objectable {
	explicit operator bool() const { return type != NoObject; }
	void		update();
private:
	void		patrol();
};
void add_air_unit(point pt, objectn id, direction d, unsigned char player);