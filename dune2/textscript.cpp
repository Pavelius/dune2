#include "bsdata.h"
#include "textscript.h"

static bool parse_script(stringbuilder& sb, const char* id) {
	for(auto& e : bsdata<textscript>()) {
		if(!equal(e.id, id))
			continue;
		e.proc(sb);
		return true;
	}
	return false;
}

static void custom_string(stringbuilder& sb, const char* id) {
	if(parse_script(sb, id))
		return;
	sb.add(getnm(id));
}

void initialize_strings() {
	stringbuilder::custom = custom_string;
}
