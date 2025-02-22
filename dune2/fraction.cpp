#include "bsdata.h"
#include "fraction.h"
#include "resid.h"

fractionn last_fraction;

BSDATA(fractioni) = {
	{"NoFraction", NONE, 0, 0, 3, MENSHPM, 0},
	{"Atreides", FARTR, 0, 0, 0, MENSHPA, 2},
	{"Harkonens", FHARK, 0, 0, 1, MENSHPH, 1},
	{"Ordos", FORDOS, 0, 0, 2, MENSHPO, 3},
};
assert_enum(fractioni, Ordos)

const fractioni& fractionable::getfraction() const {
	return bsdata<fractioni>::elements[fraction];
}

const char* fractionable::getfractionsuffix() const {
	switch(fraction) {
	case Atreides: return "a";
	case Harkonens: return "h";
	case Ordos: return "o";
	default: return 0;
	}
}