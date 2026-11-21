#include "object.h"

#pragma once

struct scenarioi {
	struct armymove {
		objectn fraction;
		unsigned char message;
		unsigned char regions[8];
	};
	struct armyorder {
		unsigned char region;
		unsigned char frame;
		unsigned char x, y;
	};
	objectf		topics;
	unsigned	harvest;
	armymove	moves[3];
	armyorder	order[3];
};
extern scenarioi scenarios[3 * 9];

const scenarioi& scenario();