#include "bsdata.h"
#include "draw.h"
#include "print.h"
#include "pushvalue.h"
#include "log_file.h"
#include "music.h"
#include "video.h"
#include "view.h"

const unsigned long time_appear = 1 * 1000;

unsigned long start_video;

void reset_video_time() {
	start_video = animate_time;
}

void video_play(const slice<videoi>& source) {
	pushvalue push_fore(draw::fore);
	pushvalue push_id(animate_id);
	pushvalue push_header(form_header);
	pushvalue push_delay(animate_delay);
	pushvalue push_once(animate_once);
	const unsigned long frame_delay = 100;
	draw::fore = color(215, 0, 0);
	for(auto& e : source) {
		animate_id = e.id;
		animate_stop = 0;
		animate_once = true;
		animate_delay = e.frame_rate;
		form_header = getnme(e.text);
		if(e.is(Appearing)) {
			if(!e.is(ContinueToNext))
				reset_form_animation();
			appear_scene(paint_video, time_appear);
		}
		if(!e.is(ContinueToNext))
			reset_form_animation();
		if(e.is(Repeated))
			animate_once = false;
		if(e.time_lenght)
			animate_stop = animate_time + e.time_lenght;
		if(!show_scene_raw(paint_video, 0, 0)) {
			disappear_scene(colors::black, time_appear);
			break;
		}
		if(e.is(Disappearing))
			disappear_scene(e.disappear, time_appear);
	}
}

static void add_time(const char* text, unsigned long time) {
	auto seconds = time / 1000;
	if(text)
		print(text);
	print("%1.2i:%2.2i", seconds / 60, seconds % 60);
}

void video_dump(const char* url, const slice<videoi>& source, unsigned long video_time) {
	create_log_file(url);
	auto index = 0;
	for(auto& e : source) {
		auto scene_time = e.time_lenght;
		if(!e.time_lenght) {
			auto ps = gres(e.id);
			auto frame_rate = e.frame_rate;
			if(!frame_rate)
				frame_rate = 200;
			scene_time += ps->count * frame_rate;
		}
		if(e.is(Appearing))
			scene_time += time_appear;
		if(e.is(Disappearing))
			scene_time += time_appear;
		print("#%1.2i", index++);
		add_time(" start ", video_time);
		add_time(" lenght ", scene_time);
		print(" show %1", bsdata<residi>::elements[e.id].id);
		println();
		video_time += scene_time;
	}
	close_log_file();
}