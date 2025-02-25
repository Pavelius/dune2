#pragma once

#include "object.h"

struct fractionable {
	objectn		fraction;
	const char*	getfractionsuffix() const;
};
extern objectn last_fraction;