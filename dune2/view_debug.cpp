#include "area.h"
#include "bsdata.h"
#include "building.h"
#include "draw.h"
#include "fix.h"
#include "pushvalue.h"
#include "rand.h"
#include "resid.h"
#include "slice.h"
#include "unit.h"
#include "view.h"
#include "view_focus.h"
#include "view_theme.h"

using namespace draw;

namespace {
struct copypaste {
	void*	data;
	size_t	size;
	explicit operator bool() const { return data != 0; }
	void release() {
		if(data)
			delete[](unsigned char*)data;
		data = 0;
		size = 0;
	}
	void copy(void* data, size_t size) {
		release();
		if(!size)
			return;
		this->data = new unsigned char[size];
		this->size = size;
		memcpy(this->data, data, size);
	}
	void paste(void* dest) {
		if(!size)
			return;
		memcpy(dest, data, size);
	}
};
}

static copypaste copy;

static void paint_background() {
	auto push_fore = fore;
	fore = colors::blue.mix(colors::black, 64);
	rectf();
	fore = push_fore;
}

static void paint_focus_rect(point size, color border, int focus, int origin, int per_line, int dy = 0) {
	focus -= origin;
	if(focus < 0)
		return;
	pushrect push;
	auto x = caret.x + (focus % per_line) * size.x - 1;
	auto y = caret.y + (focus / per_line) * size.y - 1;
	caret.x = x;
	caret.y = y;
	width = size.x + 2 + dy;
	height = size.y + 2 + dy;
	fore = border;
	rectb();
}

static void paint_sprites(resid id, color border, int index, int per_line, int line_per_screen, int image_flags) {
	auto p = gres(id);
	if(!p)
		return;
	auto push_line = caret;
	auto push_fore = fore;
	auto count = per_line;
	while(index < p->count) {
		fore = colors::white;
		image(p, index, image_flags);
		index++;
		caret.x += width;
		if((--count) == 0) {
			count = per_line;
			caret.y += height;
			caret.x = push_line.x;
			if(--line_per_screen <= 0)
				break;
		}
	}
	fore = push_fore;
	caret = push_line;
}

static void show_sprites(resid id, point start, point size) {
	pushrect push;
	pushfore push_fore;
	auto push_font = font; font = gres(FONT6);
	auto maximum = gres(id)->count;
	auto focus = 0, origin = 0, image_flags = 0;
	color backgc = color(64, 0, 128);
	color border = color(128, 128, 128);
	while(ismodal()) {
		paint_background();
		caret.x = 2; caret.y = 2;
		width = size.x; height = size.y;
		auto caret_origin = caret;
		auto per_line = (getwidth() - 4) / size.x;
		auto maximum_height = (getheight() - caret.y - texth()) / size.y;
		if(focus < 0)
			focus = 0;
		else if(focus > maximum - 1)
			focus = maximum - 1;
		if(focus < origin)
			origin = (focus / per_line) * per_line;
		else if(focus >= origin + (maximum_height)*per_line)
			origin = ((focus - ((maximum_height - 1) * per_line)) / per_line) * per_line;
		caret = caret + start;
		fore = colors::white;
		paint_sprites(id, border, origin, per_line, maximum_height, image_flags);
		caret = caret_origin;
		paint_focus_rect(size, border, focus, origin, per_line);
		caret = {1, getheight() - 8};
		auto& f = gres(id)->get(focus);
		auto pf = const_cast<sprite::frame*>(&f);
		fore = colors::white;
		text(str("index %1i (size %2i %3i center %4i %5i)", focus, f.sx, f.sy, f.ox, f.oy), -1, TextBold);
		domodal();
		switch(hot.key) {
		case KeyRight: focus++; break;
		case KeyLeft: focus--; break;
		case KeyDown: focus += per_line; break;
		case KeyUp: focus -= per_line; break;
		case KeyEscape: breakmodal(0); break;
		case 'A': pf->ox--; break;
		case 'S': pf->ox++; break;
		case 'W': pf->oy--; break;
		case 'Z': pf->oy++; break;
		case 'H': image_flags ^= ImageMirrorH; break;
		case 'V': image_flags ^= ImageMirrorV; break;
		}
		focus_input();
	}
	font = push_font;
}

static void set_pixel4(unsigned char* data, point subindex, int sn, unsigned char v) {
	auto pd = data + subindex.y * sn + subindex.x / 2;
	if(subindex.x & 1)
		*pd = (*pd & 0x0F) | (v << 4);
	else
		*pd = (*pd & 0xF0) | (v & 0x0F);
}

static void paint_font_frame(int x0, int y0, const sprite* pf, int frame, color main, int zoom, unsigned char& result_index) {
	static point subindex = {0, 0};
	static unsigned char symbol_tool = '1';
	pushrect push;
	pushfontb push_palt(TextYellow);
	font_pallette[0] = colors::green;
	font_pallette[2] = colors::green.mix(colors::black, 32); // Shadow color
	font_pallette[3] = colors::green.mix(colors::black, 128); // Light color
	font_pallette[7] = colors::green.mix(colors::red, 128);
	font_pallette[10] = colors::green.mix(colors::red, 32);
	font_pallette[12] = colors::green.mix(colors::black, 128); // Light of second color
	font_pallette[14] = colors::green.mix(colors::black, 32); // Sadow of second color
	auto push_fore = fore;
	auto pd = (unsigned char*)pf->ptr(pf->get(frame).offset);
	auto sx = pf->get(frame).sx;
	auto sy = pf->get(frame).sy;
	auto sn = (pf->get(frame).sx * 4 + 7) / 8;
	auto sn1 = ((pf->get(frame).sx + 1) * 4 + 7) / 8;
	if(subindex.x < 0)
		subindex.x = 0;
	if(subindex.x >= sx)
		subindex.x = sx - 1;
	if(subindex.y < 0)
		subindex.y = 0;
	if(subindex.y >= sy)
		subindex.y = sy - 1;
	draw::width = zoom - 1;
	draw::height = zoom - 1;
	int used_colors[16] = {0};
	result_index = 0xFF;
	auto symbol_data = pd;
	auto tolal_data = sn * sy;
	for(auto y = 0; y < sy; y++) {
		caret.y = y0 + y * zoom;
		for(auto w = 0; w < sx; w++) {
			auto index = pd[w / 2];
			if(w & 1)
				index >>= 4;
			else
				index &= 0x0F;
			fore = palt[index];
			caret.x = x0 + w * zoom;
			rectf();
			used_colors[index]++;
			if(subindex.x == w && subindex.y == y)
				result_index = index;
		}
		pd += sn;
	}
	fore = colors::gray;
	caret.x = x0 + subindex.x * zoom - 1;
	caret.y = y0 + subindex.y * zoom - 1;
	width += 2; height += 2;
	rectb();
	fore = push_fore;
	caret = push.caret;
	caret.y = y0 + zoom * 16 + 2;
	caret.x = x0;
	auto used_count = 0;
	for(auto n : used_colors) {
		if(n)
			used_count++;
	}
	string sb;
	sb.adds("Used %1i indecies ", used_count);
	auto index = 0;
	for(auto n : used_colors) {
		if(n)
			sb.adds("%1i", index);
		index++;
	}
	font = gres(FONT6);
	text(sb); caret.y += texth();
	sb.clear();
	sb.addn("Tool %1i", symbol_tool);
	text(sb); caret.y += texth();
	switch(hot.key) {
	case 'E': set_pixel4(symbol_data, subindex, sn, symbol_tool); break;
	case '1': symbol_tool = 1; break;
	case '2': symbol_tool = 2; break;
	case '3': symbol_tool = 3; break;
	case '4': symbol_tool = 4; break;
	case '5': symbol_tool = 5; break;
	case '6': symbol_tool = 6; break;
	case 'A': subindex.x--; break;
	case 'S': subindex.x++; break;
	case 'W': subindex.y--; break;
	case 'Z': subindex.y++; break;
	case 'R': if(sn1 <= sn) ((sprite*)pf)->frames[frame].sx++; break;
	case Ctrl + 'C': copy.copy(symbol_data, tolal_data); break;
	case Ctrl + 'V': if(tolal_data == copy.size) copy.paste(symbol_data); break;
	case KeyBackspace: set_pixel4(symbol_data, subindex, sn, 0); break;
	}
}

static void show_font(resid id, point start, point size) {
	pushrect push;
	auto push_fore = fore;
	auto push_font = font; font = gres(FONT6);
	int focus = 0;
	auto maximum = gres(id)->count;
	auto per_line = 16;
	auto origin = 0;
	auto image_flags = 0;
	unsigned char sym_index, select_tool = 0;
	while(ismodal()) {
		paint_background();
		caret.y = 4;
		width = size.x;
		height = size.y;
		auto maximum_height = (getheight() - caret.y - texth()) / size.y;
		if(focus < 0)
			focus = 0;
		else if(focus > maximum - 1)
			focus = maximum - 1;
		if(focus < origin)
			origin = (focus / per_line) * per_line;
		else if(focus >= origin + (maximum_height)*per_line)
			origin = ((focus - ((maximum_height - 1) * per_line)) / per_line) * per_line;
		caret = caret + start;
		fore = colors::white;
		paint_sprites(id, colors::gray, origin, per_line, maximum_height, image_flags);
		paint_font_frame(
			caret.x + size.x * per_line + 2,
			caret.y,
			gres(id), focus, colors::red, 5, sym_index
		);
		paint_focus_rect(size, colors::gray, focus, origin, per_line);
		caret = {1, getheight() - 8};
		auto& f = gres(id)->get(focus);
		auto pf = const_cast<sprite::frame*>(&f);
		text(str("index %1i (size %2i %3i center %4i %5i) index=%6i", focus, f.sx, f.sy, f.ox, f.oy, sym_index), -1, TextBold);
		domodal();
		switch(hot.key) {
		case KeyRight: focus++; break;
		case KeyLeft: focus--; break;
		case KeyDown: focus += per_line; break;
		case KeyUp: focus -= per_line; break;
		case KeyEscape: breakmodal(0); break;
		case 'H': image_flags ^= TextBold; break;
		case 'V': image_flags ^= TextStroke; break;
		case Ctrl + 'S': save_sprite(id); break;
		}
		focus_input();
	}
	font = push_font;
	fore = push_fore;
}

static void random_explosion() {
	static fixn source[] = {FixBikeExplosion, FixExplosion, FixBigExplosion};
	auto n = source[rand() % sizeof(source) / sizeof(source[0])];
	// auto n = FixHitSand;
	add_effect(m2sc(area_spot), n, 0xFFFF);
}

static void show_pallette(resid id, int dy) {
	pushrect push;
	auto push_fore = fore;
	auto push_font = font; font = gres(FONT6);
	int focus = 0;
	auto pallette = (color*)gres(id)->ptr(gres(id)->get(0).pallette);
	while(ismodal()) {
		paint_background();
		caret.y = 4; caret.x += 4;
		if(focus < 0)
			focus = 0;
		else if(focus > 256 - 1)
			focus = 256 - 1;
		auto push_caret = caret;
		width = dy - 1; height = dy - 1;
		for(auto y = 0; y < 16; y++) {
			for(auto x = 0; x < 16; x++) {
				caret.x = x * (width + 1) + push_caret.x;
				caret.y = y * (height + 1) + push_caret.y;
				fore = pallette[y * 16 + x];
				rectf();
			}
		}
		caret = push_caret;
		paint_focus_rect({dy, dy}, colors::gray, focus, 0, 16, -1);
		caret = {1, getheight() - 8};
		auto value = pallette[focus];
		text(str("index %1i (0x%5.2h), color %2i, %3i, %4i", focus, value.r, value.g, value.b, focus), -1, TextBold);
		domodal();
		switch(hot.key) {
		case KeyRight: focus++; break;
		case KeyLeft: focus--; break;
		case KeyDown: focus += 16; break;
		case KeyUp: focus -= 16; break;
		case KeyEscape: breakmodal(0); break;
		}
		focus_input();
	}
	font = push_font;
	fore = push_fore;
}

static void change_debug() {
	debug_toggle = !debug_toggle;
	//area.blockcontrol();
	//area.controlwave(area_spot, allowcontrol, 32);
	//blockarea(isunitpossible);
	//blockunits(0);
}

void view_debug_input() {
	switch(hot.key) {
	case Ctrl + 'S': show_sprites(SHAPES, {0, 0}, {32, 24}); break;
	case Ctrl + 'I': show_sprites(ICONS, {0, 0}, {16, 16}); break;
	case Ctrl + 'A': show_sprites(UNITS, {8, 8}, {16, 16}); break;
	case Ctrl + 'B': show_sprites(UNITS1, {8, 8}, {16, 16}); break;
	case Ctrl + 'C': show_sprites(UNITS2, {8, 8}, {16, 16}); break;
	case Ctrl + 'F': show_font(FONT10, {4, 3}, {12, 12}); break;
	case Ctrl + 'M': show_sprites(MOUSE, {0, 0}, {16, 16}); break;
	case Ctrl + 'P': show_pallette(SHAPES, 8); break;
	case 'B': area.set(area_spot, Blood); break;
	case 'D': change_debug(); break;
	case 'E': random_explosion(); break;
	case 'F': area.set(area_spot, d100() < 60 ? Body : Bodies); break;
	}
}