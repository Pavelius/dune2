#pragma once

#include "array.h"

struct tree : array {
	enum treef : unsigned char {
		Group, Marked,
	};
	struct element {
		unsigned char	level, flags;
		unsigned short	param;
		void*			data;
		bool			is(treef v) const;
		void			set(treef v);
	};
	tree() : array(sizeof(element)) {}
	element*			addchild(void* data, unsigned short param);
	element*			addgroup(void* data, unsigned char level, unsigned short param);
	element*			begin() { return (element*)data; }
	const element*		end() const { return ((element*)data) + count; }
	element*			parent(element* p);
private:
	element*			last();
};