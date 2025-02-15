#include "tree.h"
#include "math.h"

bool tree::element::is(treef v) const {
	return (flags & FG(v)) != 0;
}

void tree::element::set(treef v) {
	flags |= FG(v);
}

tree::element* tree::addchild(void* data, unsigned short param) {
	auto pr = last();
	if(!pr)
		return 0;
	auto p = (element*)add();
	if(pr->is(Group))
		p->level = pr->level + 1;
	else
		p->level = pr->level;
	p->data = data;
	p->param = param;
	p->flags = 0;
	return p;
}

tree::element* tree::last() {
	if(count)
		return ((element*)data) + (count - 1);
	return 0;
}

tree::element* tree::addgroup(void* data, unsigned char level, unsigned short param) {
	element* p;
	element* pr = last();
	if(pr && pr->is(Group))
		p = pr;
	else {
		p = (element*)add();
		p->set(Group);
	}
	p->level = level;
	p->data = data;
	p->param = param;
	return p;
}

tree::element* tree::parent(element* p) {
	auto pb = begin();
	if(!pb)
		return 0;
	auto need_level = p->level - 1;
	while((--p) > pb) {
		if(p->level == need_level)
			return p;
	}
	return 0;
}