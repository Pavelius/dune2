#pragma once

#include "color.h"
#include "point.h"

typedef void(*fnevent)();

enum resid : short unsigned;
enum objectn : unsigned char;

extern unsigned long animate_time, animate_delay, animate_stop;
extern resid animate_id;
extern bool animate_once;
extern bool debug_toggle;
extern int build_current;
extern const char* form_header;

int get_frame(unsigned long resolution = 0); // Get animation frame
color get_flash(color main, color back, int base, int range, int resolution = 10);
extern color pallette[256];

void appear_scene(fnevent paint, unsigned long milliseconds = 0);
void disappear_scene(unsigned long milliseconds = 0);
void hilite_unit_orders();
void initialize_view(const char* title, fnevent main_scene);
void open_mentat();
void open_building();
void paint_background(color v);
void paint_build_shape(int x, int y, objectn build);
void paint_main_map();
void paint_main_map_choose_terrain();
void paint_main_map_choose_placement();
void paint_spice();
void paint_video();
void reset_form_animation();
long show_scene(fnevent before_paint, fnevent input, void* focus);
long show_scene_raw(fnevent before_paint, fnevent input, void* focus);
bool time_animate(unsigned long& value, unsigned long duration, unsigned long pause = 20);
