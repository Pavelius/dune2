#pragma once

typedef void(*fnlistrow)(int index, void* data);

void correct_list(int& origin, int& current, int maximum, int per_page);
void paint_list(int& origin, int& current, int maximum, void* elements, size_t element_size, int row_height, fnlistrow proc);
void paint_list_and_scroll(int& origin, int& current, int maximum, void* elements, size_t element_size, int row_height, fnlistrow proc);