#include "video.h"
#include "view.h"

static void play_video(resid id, unsigned long delay = 0, bool once = false) {
	if(!delay)
		delay = 96;
	auto push_id = animate_id; animate_id = id;
	auto push_delay = animate_delay; animate_delay = delay;
	auto push_once = animate_once; animate_once = once;
	show_scene(paint_video, 0, 0);
	animate_once = push_once;
	animate_delay = push_delay;
	animate_id = push_id;
}

void play_video(const videoi* source) {
	for(auto p = source; *p; p++) {

	}
}