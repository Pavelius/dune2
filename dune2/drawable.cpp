#include "adat.h"
#include "bsdata.h"
#include "drawable.h"

typedef adat<drawable*, 256> drawablea;

static void add_objects(drawablea& objects, drawtypei& e, const rect& rc) {
	auto sz = e.source->element_size;
	auto pe = e.element + sz;
	for(auto p = e.element; p < pe; p = (drawable*)((char*)p+sz)) {
		if(!p->position.in(rc))
			continue;
		objects.add(p);
	}
}

void draw_objects() {
	adat<drawable*, 256> objects;
	rect rc;
	for(auto& e : bsdata<drawtypei>())
		add_objects(objects, e, rc);
}