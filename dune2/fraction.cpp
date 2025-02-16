#include "bsdata.h"
#include "fraction.h"
#include "resid.h"

fractionn last_fraction;

BSDATA(fractioni) = {
	{"NoFraction", NONE, 0, 3, MENSHPM},
	{"Atreides", FARTR, 0, 0, MENSHPA},
	{"Harkonens", FHARK, 0, 1, MENSHPH},
	{"Ordos", FORDOS, 0, 2, MENSHPO},
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