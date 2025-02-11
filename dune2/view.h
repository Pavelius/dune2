#pragma once

typedef void(*fnevent)();

enum resid : short unsigned;

extern unsigned long animate_time;
extern resid mentat_subject;
extern int glyph_offset_width;

int get_frame(unsigned long resolution = 0); // Get animation frame

void initialize_view(const char* title, fnevent main_scene);
void paint_main_map();
void paint_mentat();
void paint_mentat_silent();
void show_scene(fnevent before_paint, fnevent input, void* focus);
void set_font(resid v);
