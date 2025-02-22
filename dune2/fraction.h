#pragma once

#include "topicable.h"

enum fractionn : unsigned char {
	NoFraction, Atreides, Harkonens, Ordos,
};
extern fractionn last_fraction;
struct fractioni : topicable {
	int			mentat_frame;
	resid		mentat_face;
	int			default_color;
};
struct fractionable {
	fractionn	fraction;
	const fractioni& getfraction() const;
	const char*	getfractionsuffix() const;
};