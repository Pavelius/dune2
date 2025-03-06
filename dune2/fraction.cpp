#include "bsdata.h"
#include "fraction.h"
#include "object.h"

objectn last_fraction;

const char* fractionable::getfractionsuffix() const {
	switch(fraction) {
	case Atreides: return "a";
	case Harkonens: return "h";
	case Ordos: return "o";
	default: return 0;
	}
}

const char* fractionable::getfractionid() const {
	return getido(fraction);
}