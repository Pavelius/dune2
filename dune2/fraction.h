#pragma once

enum objectn : unsigned char;

struct fractionable {
	objectn		fraction;
	const char*	getfractionsuffix() const;
};
extern objectn last_fraction;