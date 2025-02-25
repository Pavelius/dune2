#include "bsdata.h"
#include "draw.h"
#include "fraction.h"
#include "player.h"
#include "pushvalue.h"
#include "resid.h"
#include "object.h"
#include "objecta.h"
#include "timer.h"
#include "music.h"
#include "view.h"
#include "view_list.h"
#include "view_theme.h"

using namespace draw;

static const char* form_id;
static unsigned long eye_clapping, eye_show_cursor, first_action;
static fnevent paint_mentat_proc;

static void paint_eyes() {
	auto rid = getres(last_fraction);
	auto frame = 0;
	if(time_animate(eye_clapping, 1, 16))
		frame = 4;
	else if(time_animate(eye_show_cursor, 30, 40)) {
		if(hot.mouse.x > 150)
			frame = 2;
		else if(hot.mouse.x < 30)
			frame = 1;
		else if(hot.mouse.y > 130)
			frame = 3;
	}
	switch(rid) {
	case MENSHPA: image(caret.x + 40, caret.y + 80, gres(rid), frame, 0); break;
	case MENSHPH: image(caret.x + 32, caret.y + 88, gres(rid), frame, 0); break;
	case MENSHPO: image(caret.x + 16, caret.y + 80, gres(rid), frame, 0); break;
	case MENSHPM: image(caret.x + 64, caret.y + 80, gres(rid), frame, 0); break;
	}
}

static void paint_action() {
	auto rid = getres(last_fraction);
	auto frame = 0;
	auto ps = gres(rid);
	if(time_animate(first_action, 20, 32)) {
		frame = 12 + (animate_time - first_action) / 200;
		if(frame >= ps->count)
			frame = 0;
	}
	if(!frame)
		return;
	switch(rid) {
	case MENSHPA: image(caret.x + 72, caret.y + 152, ps, frame, 0); break;
	case MENSHPO: image(caret.x + 88, caret.y + 144, ps, frame, 0); break;
	}
}

static void paint_speaking_mouth() {
	static int speak_frames[] = {5, 6, 5, 6, 5, 6, 5, 6, 7, 6, 5, 6, 7, 8, 9};
	auto rid = getres(last_fraction);
	auto frame = speak_frames[get_frame() % (sizeof(speak_frames) / sizeof(speak_frames[0]))];
	switch(rid) {
	case MENSHPA: image(caret.x + 40, caret.y + 96, gres(rid), frame, 0); break;
	case MENSHPH: image(caret.x + 32, caret.y + 104, gres(rid), frame, 0); break;
	case MENSHPO: image(caret.x + 16, caret.y + 96, gres(rid), frame, 0); break;
	case MENSHPM: image(caret.x + 56, caret.y + 96, gres(rid), frame, 0); break;
	}
}

static void rectf_back() {
	rectf(colors::gray.mix(colors::black));
}

static void paint_stat_info() {
	if(!form_id)
		return;
	auto pi = getnme(ids(form_id, "Capacity"));
	if(!pi)
		return;
	pushrect push;
	string sb;
	setoffset(4, 4);
	auto maximum_line = get_frame(1000);
	while(pi[0]) {
		if(maximum_line-- <= 0)
			break;
		sb.clear(); pi = sb.psline(pi);
		text(sb, -1, TextStroke);
		caret.y += texth();
	}
}

static void paint_mentat_subject() {
	auto ps = gres(animate_id);
	if(ps && ps->count)
		image(caret.x, caret.y, ps, get_frame(400) % ps->count, 0);
	paint_stat_info();
}

static void paint_mentat_subject_ni() {
	auto ps = gres(animate_id);
	if(ps && ps->count)
		image(caret.x, caret.y, ps, get_frame(400) % ps->count, 0);
}

static bool allow_brief_row(int index, void* data) {
	auto type = *((objectn*)data);
	return getparent(type) != NoObject;
}

static void paint_brief_row(int index, void* data) {
	pushrect push;
	auto push_fore = fore;
	auto push_fore_stroke = fore_stroke;
	auto type = *((objectn*)data);
	if(getparent(type)) {
		caret.x += 12;
		width -= 12;
	} else
		fore = color(80, 120, 212);
	text(getnmo(type), -1, 0);
	fore_stroke = push_fore_stroke;
	fore = push_fore;
}

static void paint_mentat_list() {
	pushvalue push_font(font, gres(FONT8));
	static int origin;
	rectf_back();
	setoffset(4, 4);
	caret.x += 16; width -= 16 + 8;
	auto header_height = texth();
	auto push_fore = fore; fore = color(180, 228, 252);
	text(str("%1:", getnm("ChooseSubject"))); caret.y += header_height; height -= header_height;
	fore = push_fore;
	font = gres(FONT6);
	setoffset(1, 1);
	paint_list_and_scroll(origin, subjects.count, subjects.data, sizeof(subjects.data[0]), 7, paint_brief_row, allow_brief_row, buttonparam);
}

static void paint_mentat_information() {
	pushrect push;
	caret.x += 128; caret.y += 48;
	width = 184; height = 112;
	paint_mentat_proc();
}

static void paint_mentat_back() {
	paint_mentat_information();
	auto rid = getres(last_fraction);
	switch(rid) {
	case MENSHPA: image(caret.x + 128, caret.y + 128, gres(rid), 10, 0); break;
	case MENSHPH: image(caret.x + 128, caret.y + 104, gres(rid), 10, 0); break;
	case MENSHPO: image(caret.x + 128, caret.y + 128, gres(rid), 10, 0); break;
	}
}

static void paint_background() {
	paint_background(colors::black);
	switch(last_fraction) {
	case Atreides: image(gres(MENTATS), 0, 0); break;
	case Harkonens: image(gres(MENTATS), 1, 0); break;
	case Ordos: image(gres(MENTATS), 2, 0); break;
	default: image(gres(MENTATS), 3, 0); break;
	}
}

static void paint_exit() {
	pushrect push;
	pushtheme theme(ButtonDark);
	pushfontb theme_font(TextYellow);
	pushfont push_font(gres(FONT16));
	caret.x += 198; caret.y += 169; width = 52;
	if(button(getnm("Exit"), KeyEscape, AlignCenterCenter | ImagePallette, true, texth() + 5, form_press_effect))
		execute(update_buttonparam, 0);
}

static void paint_proceed_repeat() {
	pushrect push;
	pushtheme theme(ButtonDark);
	pushfontb theme_font(TextYellow);
	pushfont push_font(gres(FONT16));
	caret.x += 168; caret.y += 169; width = 68;
	if(button(getnm("Proceed"), KeyEnter, AlignCenterCenter | ImagePallette, true, texth() + 5, form_press_effect))
		execute(update_buttonparam, 1);
	caret.x += 72;
	if(button(getnm("Repeat"), KeyEscape, AlignCenterCenter | ImagePallette, true, texth() + 5, form_press_effect))
		execute(update_buttonparam, 0);
}

static void paint_form_header() {
	if(!form_header)
		return;
	pushrect push;
	pushvalue push_font(font);
	font = gres(FONT8);
	setoffset(4, 4);
	texta(form_header, AlignLeft | TextStroke);
}

static void input_accept() {
	if((hot.key == MouseLeft && !hot.pressed) || hot.key == KeyEnter || hot.key == KeySpace)
		execute(buttonok);
}

static void paint_mentat_speaking() {
	paint_background();
	paint_eyes();
	paint_speaking_mouth();
	paint_mentat_back();
	paint_form_header();
	input_accept();
}

void paint_mentat_silent() {
	paint_background();
	paint_eyes();
	paint_mentat_back();
	paint_action();
}

static int compare_topic_by_name(const void* v1, const void* v2) {
	auto i1 = *((objectn*)v1);
	auto i2 = *((objectn*)v2);
	return szcmp(getnmo(i1), getnmo(i2));
}

static void add_group(objectn parent) {
	auto i0 = subjects.count;
	subjects.add(parent);
	auto i1 = subjects.count;
	subjects.addchild(parent);
	auto i2 = subjects.count;
	if(i1 == i2) {
		subjects.count = i0;
		return;
	}
	qsort(subjects.data + i1, i2 - i1, sizeof(subjects.data[0]), compare_topic_by_name);
}

static void update_topics() {
	subjects.clear();
	add_group(House);
	add_group(Unit);
	add_group(Building);
}

static void* choose_mentat_topic() {
	pushvalue push_proc(paint_mentat_proc, paint_mentat_list);
	update_topics();
	return (void*)show_scene(paint_mentat_silent, paint_exit, 0);
}

static void show_mentat_subject(const char* id, resid rid) {
	string sb;
	pushvalue push_header(form_header);
	pushvalue push_id(form_id, id);
	pushvalue push_animation(animate_id, rid);
	pushvalue push_proc(paint_mentat_proc, paint_mentat_subject);
	auto pi = getnme(ids(id, getido(mainplayer().fraction), "Info"));
	if(!pi)
		pi = getnme(ids(id, "Info"));
	reset_form_animation();
	if(pi) {
		while(pi[0]) {
			sb.clear(); pi = sb.psline(pi);
			if(!sb)
				break;
			form_header = sb;
			if(!show_scene_raw(paint_mentat_speaking, 0, 0))
				return;
		}
	}
	show_scene_raw(paint_mentat_silent, paint_exit, 0);
}

void show_scenario_prompt(const char* id, resid rid, int level) {
	pushvalue push_fraction(last_fraction, mainplayer().fraction);
	pushvalue push_header(form_header);
	pushvalue push_animation(animate_id, rid);
	pushvalue push_proc(paint_mentat_proc, paint_mentat_subject_ni);
	auto pi = getnme(str("%1%2%3.2i", getido(mainplayer().fraction), id, level));
	if(!pi)
		return;
	song_play(str("mentat%1", mainplayer().getfractionsuffix()));
	reset_form_animation();
	string sb;
	while(true) {
		auto ps = pi;
		while(pi[0]) {
			sb.clear(); ps = sb.psline(ps);
			if(!sb)
				break;
			form_header = sb;
			if(!show_scene_raw(paint_mentat_speaking, 0, 0))
				return;
		}
		if(show_scene_raw(paint_mentat_silent, paint_proceed_repeat, 0))
			break;
	}
	music_play(0);
}

void open_mentat() {
	pushvalue push_fraction(last_fraction, mainplayer().fraction);
	song_play(str("mentat%1", mainplayer().getfractionsuffix()));
	while(true) {
		auto line = (objectn*)choose_mentat_topic();
		if(!line)
			break;
		auto subject = *line;
		show_mentat_subject(getido(subject), getavatar(subject));
	}
	music_play(0);
}