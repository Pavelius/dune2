#pragma once

#include "array.h"
#include "point.h"

typedef void(*fnevent)();

struct draworder;
struct drawrenderi;
struct drawable {
	point			screen; // Drawing screen position
	unsigned char	render; // Drawing render index
	unsigned short	param; // Painting frame and other parameters (2 byte)
	unsigned long	start_time; // Start animation
	explicit operator bool() const { return start_time != 0; }
	void			clearobject(); // Unsafe, but work on non-virtual classes if you define `clear` proc.
	const drawrenderi& getrender() const;
	unsigned char	renderindex() const; // Calculate render index
	unsigned short	objectindex() const;
};
struct draweffect : drawable {
	point			from, to;
};
struct drawrenderi {
	const char*		id; // Object name
	array&			source; // Reference to source objects source array
	drawable*		element; // First element offset.
	fnevent			paint; // Paint procedure for drawable object. Can use caret and last_object.
	fnevent			clear; // After clearobject() called this. Can use last_object.
};
extern drawable* last_object;
extern int object_padding;

void add_effect(point screen, short unsigned param, unsigned long start_time);
void add_effect(point from, point to, short unsigned param, unsigned long start_time);
void paint_objects(point camera);