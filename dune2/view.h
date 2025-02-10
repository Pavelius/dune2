#pragma once

typedef void(*fnevent)();

enum resid : short unsigned;
enum drawtypen : unsigned char {
	DrawUnit, DrawEffect
};

extern unsigned long current_tick;
extern resid mentat_subject; // Picture on back back of mentat
extern int glyph_offset_width;

int get_frame(unsigned long resolution = 0); // Get animation frame

void initialize_view(const char* title, fnevent main_scene);
void paint_main_map();
void paint_mentat();
void paint_mentat_silent();
void show_scene(fnevent before_paint, fnevent input, void* focus);
void set_font(resid v);
