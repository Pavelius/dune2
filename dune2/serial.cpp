#include "archive.h"
#include "aircraft.h"
#include "area.h"
#include "draw.h"
#include "game.h"
#include "building.h"
#include "stringbuilder.h"
#include "unit.h"

void game_file(char* result, const char* folder, const char* name, const char* ext, int index) {
	io::file::home(result);
	stringbuilder sb(result, result + 511); sb.setend();
#ifdef _MSC_VER
	sb.add("\\Dune II");
#else
	sb.add("/.local/share/dune2");
#endif
	io::file::makedir(result);
	if(folder) {
		sb.addv(FILE_SEP, 0);
		sb.addv(folder, 0);
		io::file::makedir(result);
	}
	if(name) {
		sb.addv(FILE_SEP, 0);
		sb.add(name, index);
		if(ext) {
			sb.addv(".", 0);
			sb.addv(ext, 0);
		}
	}
}

static bool serial_game_url(const char* url, bool writemode) {
	io::file file(url, writemode ? StreamWrite : StreamRead);
	if(!file)
		return false;
	archive e(file, writemode);
	if(!e.signature("SAV"))
		return false;
	e.set(game);
	e.set(player_index);
	e.set(player_human);
	e.set(area);
	e.set(area_origin);
	e.set(bsdata<playeri>::source);
	e.set(bsdata<unit>::source);
	e.set(bsdata<aircraft>::source);
	e.set(bsdata<building>::source);
	e.set(bsdata<draweffect>::source);
	return true;
}

static bool serial_game(const char* id, bool writemode) {
	char path[512]; game_file(path, "saves", id, "sav", -1);
	return serial_game_url(path, writemode);
}

void save_game(const char* id) {
	serial_game(id, true);
}

void load_game(const char* id) {
	serial_game(id, false);
}