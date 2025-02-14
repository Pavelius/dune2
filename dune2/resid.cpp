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
	{"mouse", root},
	{"screen", root},
	{"4slab", mentat},
	{"barrac", mentat},
	{"carryall", mentat},
	{"construc", mentat},
	{"westwood", mentat},
	{"intro1", mentat},
	{"intro2", mentat},
	{"intro3", mentat},
	{"intro4", mentat},
	{"intro5", mentat},
	{"intro6", mentat},
	{"intro7a", mentat},
	{"intro7b", mentat},
	{"intro8a", mentat},
	{"intro8b", mentat},
	{"intro8c", mentat},
	{"intro9", mentat},
	{"intro10", mentat},
	{"intro11", mentat},
	{"units", root},
	{"units1", root},
	{"units2", root},
};
assert_enum(residi, UNITS2)

sprite* residi::get() {
	if(data)
		return data;
	if(!data && !error) {
		char temp[260]; szurl(temp, sizeof(temp), folder, 0, 0, id, "pma");
		data = (sprite*)loadb(temp, &size);
		error = (data != 0);
	}
	return data;
}

const sprite* gres(resid i) {
	if(i == NONE)
		return 0;
	return bsdata<residi>::elements[i].get();
}

const sprite* gres(const char* id) {
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

void save_sprite(resid id) {
	auto& e = bsdata<residi>::elements[id];
	if(!e.size || !e.error)
		return;
	char temp[260]; szurl(temp, sizeof(temp), e.folder, 0, 0, e.id, "pma");
	io::file file(temp, StreamWrite);
	file.write(e.data, e.size);
}