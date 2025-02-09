#include "math.h"
#include "pointc.h"

int pointc::range(pointc v) const {
	return imax(iabs(v.x - x), iabs(v.y - y));
}