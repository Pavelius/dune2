#include "area.h"
#include "midi.h"
#include "music.h"
#include "log.h"
#include "rand.h"
#include "stringbuilder.h"
#include "textscript.h"
#include "timer.h"
#include "view_map.h"
#include "view_theme.h"

#ifdef _DEBUG
void util_main();
#endif // _DEBUG

void main_menu();

int main() {
	start_random_seed = getcputime();
	//start_random_seed = 598697093;
	srand(start_random_seed);
	initialize_translation();
	if(log::errors)
		return -1;
	area_initialization();
	music_initialize();
	// midi_open();
	// song_play("ambient06");
#ifdef _DEBUG
	util_main();
#endif // _DEBUG
	initialize_strings();
	pushtheme push(ButtonLight);
	initialize_view("Dune 2 (Remake)", main_menu);
}

#ifdef _MSC_VER
int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main();
}
#endif