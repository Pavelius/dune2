#pragma once

typedef void(*fnevent)();

enum resid : short unsigned;

extern unsigned long animate_time, animate_delay;
extern resid animate_id;
extern bool animate_once, animate_continue;

int get_frame(unsigned long resolution = 0); // Get animation frame

void human_unit_attack();
void human_unit_move();
void human_unit_stop();
void initialize_view(const char* title, fnevent main_scene);
void mouse_unit_move();
void paint_main_map();
void paint_main_map_choose_terrain();
void paint_mentat();
void paint_mentat_silent();
void paint_video();
long show_scene(fnevent before_paint, fnevent input, void* focus);
