#include "bsdata.h"
#include "io_stream.h"
#include "resid.h"

static const char* root = "art/core";
static const char* mentat = "art/mentat";

BSDATA(residi) = {
	{"NONE"},
	{"new6p", root},
	{"new8p", root},
	{"new10p", root},
	{"std3p", root},
	{"intro16", root},
	{"mentats", root},
	{"menshpa", root},
	{"menshph", root},
	{"menshpo", root},
	{"menshpm", root},
	{"shapes", root},
	{"icons", root},
	{"arrows", root},
	{"screen", root},
	{"4slab", mentat},
	{"barrac", mentat},
	{"carryall", mentat},
	{"construc", mentat},
	{"units", root},
};
assert_enum(residi, UNITS)

sprite* residi::get() {
	if(data)
		return data;
	if(!data && !error) {
		char temp[260]; szurl(temp, sizeof(temp), folder, 0, 0, id, "pma");
		data = (sprite*)loadb(temp);
		if(!data) {
			szurl(temp, sizeof(temp), folder, 0, 0, id, "fnt");
			data = (sprite*)loadb(temp);
		}
		error = (data != 0);
	}
	return data;
}

sprite* gres(resid i) {
	if(i == NONE)
		return 0;
	return bsdata<residi>::elements[i].get();
}

sprite* gres(const char* id) {
	auto p = bsdata<residi>::find(id);
	if(!p)
		return 0;
	return p->get();
}

void clear_sprites() {
	for(auto& e : bsdata<residi>()) {
		if(e.data) {
			delete[]((char*)e.data);
			e.data = 0;
		}
	}
}
