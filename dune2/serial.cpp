#include "archive.h"
#include "area.h"
#include "draw.h"
#include "game.h"
#include "building.h"
#include "stringbuilder.h"
#include "unit.h"

static bool serial_game_url(const char* url, bool writemode) {
	io::file file(url, writemode ? StreamWrite : StreamRead);
	if(!file)
		return false;
	archive e(file, writemode);
	if(!e.signature("SAV"))
		return false;
	e.set(game);
	e.set(area);
	e.set(draw::camera);
	e.set(bsdata<playeri>::source);
	e.set(bsdata<unit>::source);
	e.set(bsdata<building>::source);
	e.set(bsdata<draweffect>::source);
	return true;
}

static bool serial_game(const char* id, bool writemode) {
	string sb; sb.add("saves/%1.sav", id);
	return serial_game_url(sb, writemode);
}

void save_game(const char* id) {
	serial_game(id, true);
}

void load_game(const char* id) {
	serial_game(id, false);
}