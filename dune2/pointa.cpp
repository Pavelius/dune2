#include "pointa.h"

point pointa::nearest(point position, int maximum_range) {
	auto result_mark = maximum_range;
	if(!result_mark)
		result_mark = 256 * 256 * 256;
	point result = {-10000, -10000};
	for(auto v : *this) {
		auto m = v.range(position);
		if(result_mark >= m) {
			result_mark = m;
			result = v;
		}
	}
	return result;
}

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

void pointa::select(const rect& rc, areai::fntest proc) {
	for(auto y = rc.y1; y < rc.y2; y++) {
		for(auto x = rc.x1; x < rc.x2; x++) {
			auto v = point(x, y);
			if(proc(v))
				add(v);
		}
	}
}

void pointa::select(const rect& rc, areaf id, unsigned char player, bool value) {
	for(auto y = rc.y1; y < rc.y2; y++) {
		for(auto x = rc.x1; x < rc.x2; x++) {
			auto v = point(x, y);
			if(area.is(v, player, id) == value)
				add(v);
		}
	}
}