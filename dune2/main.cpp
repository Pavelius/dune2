#include "area.h"
#include "music.h"
#include "log.h"
#include "rand.h"
#include "stringbuilder.h"
#include "timer.h"
#include "view.h"
#include "view_theme.h"

#ifdef _DEBUG
void util_main();
#endif // _DEBUG

void main_menu();

int main() {
	start_random_seed = getcputime();
	// start_random_seed = 122397875;
	srand(start_random_seed);
	initialize_translation();
	if(log::errors)
		return -1;
	area_initialization();
	music_initialize();
	// song_play("ambient06");
#ifdef _DEBUG
	util_main();
#endif // _DEBUG
	pushtheme push(ButtonLight);
	initialize_view("Dune 2 (Remake)", main_menu);
}

#ifdef _MSC_VER
int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main();
}
#endif