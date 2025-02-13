#include "draw.h"
#include "pushvalue.h"
#include "music.h"
#include "video.h"
#include "view.h"

unsigned long start_video;

void reset_video_time() {
	start_video = animate_time;
}

void play_video(const slice<videoi>& source) {
	pushvalue push_fore(draw::fore);
	pushvalue push_id(animate_id);
	pushvalue push_delay(animate_delay);
	pushvalue push_once(animate_once);
	pushvalue push_header(form_header);
	const unsigned long time_appear = 1 * 1000;
	const unsigned long frame_delay = 100;
	draw::fore = color(215, 0, 0);
	for(auto& e : source) {
		if(e.music_start)
			song_play(e.music_start);
		animate_id = e.id;
		animate_stop = 0;
		animate_once = true;
		animate_delay = e.frame_rate;
		form_header = getnme(e.text);
		if(!e.is(ContinueToNext))
			reset_form_animation();
		if(e.is(Appearing))
			appear_scene(paint_video, time_appear);
		if(e.is(Repeated))
			animate_once = false;
		else if(e.time_lenght)
			animate_stop = animate_time + e.time_lenght;
		if(!show_scene_raw(paint_video, 0, 0)) {
			disappear_scene(time_appear);
			break;
		}
		if(e.is(Disappearing))
			disappear_scene(time_appear);
	}
}