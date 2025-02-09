#include "io_stream.h"
#include "print.h"

static io::file file;
static fnprintcallback push_proc;

static void log_file_print(const char* format) {
	file << format;
}

void create_log_file(const char* url) {
	push_proc = print_proc;
	print_proc = log_file_print;
	file.create(url, StreamWrite | StreamText);
}

void close_log_file() {
	file.close();
	print_proc = push_proc;
}