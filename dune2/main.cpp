#include "area.h"
#include "draw.h"
#include "game.h"
#include "rand.h"
#include "stringbuilder.h"
#include "textscript.h"
#include "timer.h"
#include "view_map.h"
#include "view_theme.h"

void main_menu();
void util_main();

int main() {
	start_random_seed = getcputime();
	// start_random_seed = 598697093;
	xrand_seed = start_random_seed;
	area_initialization();
#ifdef _DEBUG
	make_pallette();
	util_main();
#endif // _DEBUG
	make_pallette();
	initialize_strings();
	pushtheme push(ButtonLight);
	initialize_view(getnm(ProgramTitle), main_menu);
	run_next_scene();
}

#ifdef _MSC_VER
int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main();
}
#endif