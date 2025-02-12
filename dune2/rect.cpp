#include "math.h"
#include "rect.h"

void rect::normalize() {
	if(x1 > x2)
		iswap(x1, x2);
	if(y1 > y2)
		iswap(y1, y2);
}