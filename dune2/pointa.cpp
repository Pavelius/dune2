#include "area.h"
#include "pointa.h"

void pointa::selectfree() {
	auto ps = data;
	auto pe = ps + count_maximum;
	for(auto y = 0; y < area.maximum.y; y++) {
		for(auto x = 0; x < area.maximum.x; x++) {
			if(!path_map[y][x] || path_map[y][x] == BlockArea)
				continue;
			if(ps < pe)
				*ps++ = point(x, y);
		}
	}
	count = ps - data;
}