#include "iff.h"
#include "print.h"

int iff_number(iffit p) {
	auto size = iff_lenght(p);
	switch(size) {
	case 1: return *((unsigned char*)(p + 8));
	case 2: return *((unsigned short*)(p + 8));
	case 4: return *((unsigned int*)(p + 8));
	default: return 0;
	}
}

static const char* iff_name(char* result, iffit p) {
	result[0] = p[0];
	result[1] = p[1];
	result[2] = p[2];
	result[3] = p[3];
	result[4] = 0;
	if(result[3] == 0x20)
		result[3] = 0;
	return result;
}

const char* iff_name(iffit p) {
	static char name[5];
	return iff_name(name, p);
}

const char* iff_type_name(iffit p) {
	static char name[5];
	return iff_name(name, p + 8);
}

static iffit iff_print(iffit pf, iffit p, iffit pe, int level) {
	while(p < pe) {
		for(auto i = 0; i < level; i++)
			printv("  ");
		if(iff(p, "FORM") || iff(p, "CAT ") || iff(p, "LIST")) {
			println("%1:%2", iff_name(p), iff_type_name(p));
			p = iff_print(pf, iff_child(p), iff_next(p), level + 1);
		} else {
			println("%1 size %2i", iff_name(p), iff_lenght(p));
			p = iff_next(p);
		}
	}
	return p;
}

void iff_print(iffit p, size_t lenght) {
	iff_print(p, p, p + lenght, 0);
}