#include "draw.h"
#include "pushvalue.h"
#include "music.h"
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

void play_video(const slice<videoi>& source) {
	pushvalue push_id(animate_id);
	pushvalue push_delay(animate_delay);
	pushvalue push_once(animate_once);
	const unsigned long time_appear = 1 * 1000;
	const unsigned long frame_delay = 100;
	for(auto& e : source) {
		if(e.music_start)
			song_play(e.music_start);
		animate_id = e.id;
		if(!e.is(ContinueToNext))
			reset_form_animation();
		if(e.is(Appearing))
			appear_scene(paint_video, time_appear);
		animate_stop = 0;
		animate_once = true;
		animate_delay = e.frame_rate;
		if(e.is(Repeated))
			animate_once = false;
		else if(e.time_lenght) {
			animate_stop = animate_time + e.time_lenght;
			animate_once = false;
		}
		if(!show_scene_raw(paint_video, 0, 0)) {
			disappear_scene(time_appear);
			break;
		}
		if(e.is(Disappearing))
			disappear_scene(time_appear);
	}
}