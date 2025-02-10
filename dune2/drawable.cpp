#include "adat.h"
#include "bsdata.h"
#include "draw.h"
#include "drawable.h"

using namespace draw;

typedef adat<drawable*, 256> drawablea;
drawable* last_object;

static void add_objects(drawablea& objects, drawtypei& e, const rect& rc) {
	if(!e.source)
		return;
	auto sz = e.source->element_size;
	auto pe = (unsigned char*)e.element + sz * e.source->count;
	for(auto p = e.element; (unsigned char*)p < pe; p = (drawable*)((char*)p + sz)) {
		if(!p->position.in(rc))
			continue;
		objects.add(p);
	}
}

static int compare_unit(const void* v1, const void* v2) {
	auto p1 = *((drawable**)v1);
	auto p2 = *((drawable**)v2);
	if(p1->position.y != p2->position.y)
		return p1->position.y - p2->position.y;
	return p1->position.x - p2->position.x;
}

void paint_objects(point camera) {
	auto origin = caret;
	rect rc = {camera.x, camera.y, camera.x + width, camera.y + height};
	adat<drawable*, 256> objects;
	for(auto& e : bsdata<drawtypei>())
		add_objects(objects, e, rc);
	qsort(objects.data, objects.count, sizeof(objects.data[0]), compare_unit);
	auto push = last_object;
	for(auto p : objects) {
		last_object = p;
		caret = last_object->position - camera + origin;
		bsdata<drawtypei>::elements[p->kind].paint();
	}
	last_object = push;
}