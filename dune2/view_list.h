#pragma once

#include "color.h"
#include "point.h"

typedef void(*fnlistrow)(int index, void* data);
typedef bool(*fnlistrowallow)(int index, void* data);
typedef void(*fnevent)();

extern int list_hilite;

void correct_list(int& origin, int maximum, int per_page);
void paint_list(int& origin, int maximum, void* elements, size_t element_size, int row_height, fnlistrow proc, fnlistrowallow proc_allow, fnevent choose);
void paint_list(int& origin, int& current, int maximum, void* elements, size_t element_size, color hilite, point size, point offset, fnlistrow proc);
void paint_list_and_scroll(int& origin, int maximum, void* elements, size_t element_size, int row_height, fnlistrow proc, fnlistrowallow proc_allow, fnevent choose);