#pragma once

typedef void (*fnevent)();

struct menui {
	const char*	id;
	fnevent		proc;
	explicit operator bool() const { return id != 0; }
};
bool confirm(const char* header, const char* yes, const char* no);
long show_menu(const char* header, int menu_width, const char* cancel, const char* additional, fnevent additional_proc);
void execute_menu(const char* header, int menu_width, const char* cancel, const char* additional, fnevent additional_proc, const menui* elements);