#pragma once

typedef void(*fnevent)();

enum resid : short unsigned;

extern unsigned long animate_time, animate_delay, animate_stop;
extern resid animate_id;
extern bool animate_once;
extern bool debug_toggle;
extern const char* form_header;

int get_frame(unsigned long resolution = 0); // Get animation frame

void appear_scene(fnevent paint, unsigned long milliseconds = 0);
void button(const char* title, const void* button_data, unsigned key, unsigned flags, bool allow_set_focus, fnevent proc, long param = 0);
void create_title_font_pallette();
void disappear_scene(unsigned long milliseconds = 0);
void initialize_view(const char* title, fnevent main_scene);
void mouse_unit_move();
void paint_main_map();
void paint_main_map_choose_terrain();
void paint_mentat();
void paint_mentat_silent();
void paint_video();
void reset_form_animation();
long show_scene(fnevent before_paint, fnevent input, void* focus);
long show_scene_raw(fnevent before_paint, fnevent input, void* focus);
