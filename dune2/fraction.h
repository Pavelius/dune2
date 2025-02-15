#pragma once

#include "nameable.h"

enum resid : short unsigned;
enum fractionn : unsigned char {
	NoFraction, Atreides, Harkonens, Ordos,
};
extern fractionn last_fraction;
struct fractioni : nameable {
	int			mentat_frame;
	resid		mentat_face;
};
struct fractionable {
	fractionn	fraction;
	const fractioni& getfraction() const;
	const char*	getfractionsuffix() const;
};