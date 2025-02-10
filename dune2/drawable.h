#pragma once

#include "array.h"
#include "point.h"

typedef void(*fnevent)();

enum drawtypen : unsigned char;
struct draworder;
struct drawable {
	point			position; // Drawing position
	drawtypen		kind; // Drawing kind
	unsigned char	param; // Painting frame and other parameters
	// draworder*	addanimate(point finish);
};
struct draworder {
	unsigned short	index; // Source object index
	drawtypen		type; // Original type where source object found
	drawtypen		apply;
	point			start, finish;
	unsigned long	stamp;
};
struct drawtypei {
	const char*		id; // Object name
	fnevent			paint; // Paint procedure
	fnevent			move; // Move to procedure
	array*			source; // Point to source array
	drawable*		element; // First element offset. Delta between address of source.data and element is overhead in bytes.
};
extern drawable* last_object;

void paint_objects(point camera);