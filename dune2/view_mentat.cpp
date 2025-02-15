#include "bsdata.h"
#include "draw.h"
#include "fraction.h"
#include "player.h"
#include "pushvalue.h"
#include "resid.h"
#include "tree.h"
#include "topic.h"
#include "music.h"
#include "view.h"
#include "view_list.h"

using namespace draw;

static unsigned long eye_clapping, eye_show_cursor;
static fnevent paint_mentat_proc;
static tree topics;

static void paint_mentat_eyes() {
	auto rid = bsdata<fractioni>::elements[last_fraction].mentat_face;
	auto frame = 0;
	if(time_animate(eye_clapping, 1, 16))
		frame = 4;
	else if(time_animate(eye_show_cursor, 30, 40)) {
		if(hot.mouse.x > 200)
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

static void paint_mentat_speaking_mouth() {
	static int speak_frames[] = {5, 6, 5, 6, 5, 6, 5, 6, 7, 6, 5, 6, 7, 8, 9};
	auto rid = bsdata<fractioni>::elements[last_fraction].mentat_face;
	auto frame = speak_frames[get_frame() % (sizeof(speak_frames) / sizeof(speak_frames[0]))];
	switch(rid) {
	case MENSHPA: image(caret.x + 40, caret.y + 96, gres(rid), frame, 0); break;
	case MENSHPH: image(caret.x + 32, caret.y + 104, gres(rid), frame, 0); break;
	case MENSHPO: image(caret.x + 16, caret.y + 96, gres(rid), frame, 0); break;
	case MENSHPM: image(caret.x + 56, caret.y + 96, gres(rid), frame, 0); break;
	}
}

static void rectf_back() {
	auto push_fore = fore;
	fore = colors::gray.mix(colors::black);
	rectf();
	fore = push_fore;
}

static void paint_mentat_subject() {
	auto ps = gres(animate_id);
	if(ps && ps->count)
		image(caret.x, caret.y, ps, get_frame(400) % ps->count, 0);
}

static bool allow_brief_row(int index, void* data) {
	auto po = (tree::element*)data;
	return !po->is(tree::Group);
}

static void paint_brief_row(int index, void* data) {
	pushrect push;
	auto push_fore = fore;
	auto push_fore_stroke = fore_stroke;
	auto po = (tree::element*)data;
	auto p = (nameable*)po->data;
	caret.x += po->level * 12; width -= po->level * 12;
	if(po->level == 0)
		fore = color(80, 120, 212);
	text(p->getname(), -1, 0);
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
	text(str("%1:", getnm("ChooseSubject"))); caret.y += header_height; height -= header_height;
	font = gres(FONT6);
	setoffset(1, 1);
	paint_list_and_scroll(origin, topics.count, topics.data, topics.element_size, 7, paint_brief_row, allow_brief_row, buttonparam);
}

static void paint_mentat_information() {
	pushrect push;
	caret.x += 128; caret.y += 48;
	width = 184; height = 112;
	paint_mentat_proc();
}

static void paint_mentat_back() {
	paint_mentat_information();
	auto rid = bsdata<fractioni>::elements[last_fraction].mentat_face;
	switch(rid) {
	case MENSHPA: image(caret.x + 128, caret.y + 128, gres(rid), 10, 0); break;
	case MENSHPH: image(caret.x + 128, caret.y + 104, gres(rid), 10, 0); break;
	case MENSHPO: image(caret.x + 128, caret.y + 128, gres(rid), 10, 0); break;
	}
}

static void paint_mentat_background() {
	paint_background(colors::black);
	image(gres(MENTATS), bsdata<fractioni>::elements[last_fraction].mentat_frame, 0);
}

static void paint_mentat_exit() {
	pushrect push;
	auto push_fore = fore; fore = colors::black;
	caret.x += 200; caret.y += 180; width = 48; height = 24;
	button(getnm("Exit"), 0, KeyEscape, AlignCenterCenter, false, buttoncancel, 0);
	fore = push_fore;
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
	if((hot.key == MouseLeft && !hot.pressed) || hot.key==KeyEnter || hot.key==KeySpace)
		execute(buttonok);
}

static void paint_mentat_speaking() {
	paint_mentat_background();
	paint_mentat_eyes();
	paint_mentat_speaking_mouth();
	paint_mentat_back();
	paint_mentat_exit();
	paint_form_header();
	input_accept();
}

void paint_mentat_silent() {
	paint_mentat_background();
	paint_mentat_eyes();
	paint_mentat_back();
	paint_mentat_exit();
}

static int compare_topic_by_name(const void* v1, const void* v2) {
	auto p1 = (tree::element*)v1;
	auto p2 = (tree::element*)v2;
	return szcmp(((nameable*)p1->data)->getname(), ((nameable*)p2->data)->getname());
}

static void update_topics() {
	topics.clear();
	for(auto& e : bsdata<topici>()) {
		topics.addgroup(&e, 0, 0);
		auto i1 = topics.count;
		auto count = e.source.count;
		for(size_t i = e.start; i < count; i++)
			topics.addchild(e.source.ptr(i), 0);
		auto i2 = topics.count;
		if(i1 != i2)
			qsort(topics.ptr(i1), i2 - i1, topics.element_size, compare_topic_by_name);
	}
}

static void* choose_mentat_topic() {
	pushvalue push_proc(paint_mentat_proc, paint_mentat_list);
	update_topics();
	return (void*)show_scene(paint_mentat_silent, 0, 0);
}

static void show_mentat_subject(const char* id, resid rid) {
	string sb;
	pushvalue push_header(form_header);
	pushvalue push_animation(animate_id, rid);
	pushvalue push_proc(paint_mentat_proc, paint_mentat_subject);
	auto pi = getnme(ids(id, player->getfraction().id, "Info"));
	if(!pi)
		pi = getnme(ids(id, "Info"));
	if(pi) {
		while(pi[0]) {
			sb.clear(); pi = sb.psline(pi);
			if(!sb)
				break;
			form_header = sb;
			if(!show_scene(paint_mentat_speaking, 0, 0))
				return;
		}
	}
	show_scene(paint_mentat_silent, 0, 0);
}

void open_mentat() {
	pushvalue push_fraction(last_fraction, player->fraction);
	song_play(str("mentat%1", player->getfractionsuffix()));
	while(true) {
		auto line = (tree::element*)choose_mentat_topic();
		if(!line || !line->data)
			break;
		auto subject = (topicable*)line->data;
		show_mentat_subject(subject->id, subject->mentat_avatar);
	}
	music_play(0);
}