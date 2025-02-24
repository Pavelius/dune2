#include "adat.h"
#include "bsdata.h"
#include "draw.h"
#include "drawable.h"

BSDATAC(draweffect, 1024)

using namespace draw;

typedef adat<drawable*, 256> drawablea;
drawable *last_object;
int object_padding = 16;

void add_effect(point screen, short unsigned param, unsigned long start_time) {
	auto p = bsdata<draweffect>::addz();
	p->screen = screen;
	p->from = screen;
	p->to = screen;
	p->render = p->renderindex();
	p->param = param;
	p->start_time = start_time;
}

void add_effect(point from, point to, short unsigned param, unsigned long start_time) {
	auto p = bsdata<draweffect>::addz();
	p->screen = from;
	p->from = from;
	p->to = to;
	p->render = p->renderindex();
	p->param = param;
	p->start_time = start_time;
}

static void add_objects(drawablea& objects, drawrenderi& e, const rect& rc) {
	auto sz = e.source.element_size;
	auto pe = (unsigned char*)e.element + sz * e.source.count;
	for(auto p = e.element; (unsigned char*)p < pe; p = (drawable*)((char*)p + sz)) {
		if(!p->screen.in(rc))
			continue;
		if(!p->operator bool())
			continue;
		objects.add(p);
	}
}

static int compare_unit(const void* v1, const void* v2) {
	auto p1 = *((drawable**)v1);
	auto p2 = *((drawable**)v2);
	if(p1->screen.y != p2->screen.y)
		return p1->screen.y - p2->screen.y;
	if(p1->screen.x != p2->screen.x)
		return p1->screen.x - p2->screen.x;
	return (char)p1->render - (char)p2->render;
}

void paint_objects() {
	pushrect origin;
	rect rc = {camera.x, camera.y, camera.x + width, camera.y + height};
	rc.offset(-object_padding);
	adat<drawable*, 256> objects;
	for(auto& e : bsdata<drawrenderi>())
		add_objects(objects, e, rc);
	qsort(objects.data, objects.count, sizeof(objects.data[0]), compare_unit);
	auto push = last_object;
	for(auto p : objects) {
		last_object = p;
		caret = last_object->screen - camera + origin.caret;
		bsdata<drawrenderi>::elements[p->render].paint();
	}
	last_object = push;
}

unsigned char drawable::renderindex() const {
	for(auto& e : bsdata<drawrenderi>()) {
		if(e.source.have(this))
			return &e - bsdata<drawrenderi>::elements;
	}
	return 0xFF;
}

unsigned short drawable::objectindex() const {
	for(auto& e : bsdata<drawrenderi>()) {
		if(!e.source.have(this))
			return e.source.indexof(this);
	}
	return 0xFFFF;
}

void drawable::clearobject() {
	auto render = renderindex();
	if(render == 0xFF)
		return; // Not drawable object;
	auto push = last_object; last_object = this;
	auto& e = getrender();
	auto object_index = e.source.indexof(this);
	auto object_ptr = e.source.ptr(object_index); // To remove overhead fo like this
	memset(object_ptr, 0, e.source.element_size);
	if(e.clear)
		e.clear();
	last_object = this;
}

const drawrenderi& drawable::getrender() const {
	return bsdata<drawrenderi>::elements[render];
}