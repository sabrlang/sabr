#include "bif_io.h"

const uint32_t sabr_bif_func(io, std_out)(sabr_interpreter_t* inter) {
	sabr_value_t file;
	file.p = (uint64_t*) stdout;
	if (!sabr_interpreter_push(inter, file)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_bif_func(io, File__putc)(sabr_interpreter_t* inter) {
	sabr_value_t file, character;
	if (!sabr_interpreter_pop(inter, &file)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &character)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_fputc(inter, character, file)) return SABR_OPERR_UNICODE;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_bif_func(io, File__puts)(sabr_interpreter_t* inter) {
	sabr_value_t file, addr;
	if (!sabr_interpreter_pop(inter, &file)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &addr)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_fputs(inter, addr, file)) return SABR_OPERR_UNICODE;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_bif_func(io, File__set_cursor_pos)(sabr_interpreter_t* inter) {
	sabr_value_t x, y, file;
	if (!sabr_interpreter_pop(inter, &file)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &y)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &x)) return SABR_OPERR_STACK;
#ifdef _WIN32
	// COORD pos = { x.u, y.u };
	// HANDLE console_handle = sabr_interpreter_get_console_handle((FILE*) file.p);
	// if (!SetConsoleCursorPosition(console_handle, pos)) return SABR_OPERR_WHAT;
#else
#endif
	fprintf((FILE*) file.p, "\e[%d;%df", (int) y.i, (int) x.i);
	return SABR_OPERR_NONE;
}

const uint32_t sabr_bif_func(io, File__clear_screen)(sabr_interpreter_t* inter) {
	sabr_value_t file;
	if (!sabr_interpreter_pop(inter, &file)) return SABR_OPERR_STACK;
#ifdef _WIN32
	// HANDLE console_handle;
	// COORD origin = {0, 0};
	// DWORD count;
	// CONSOLE_SCREEN_BUFFER_INFO screen_buf;
	// console_handle = sabr_interpreter_get_console_handle((FILE*) file.p);
	// if (!GetConsoleScreenBufferInfo(console_handle, &screen_buf)) return SABR_OPERR_WHAT;
	// size_t num_chars = screen_buf.dwSize.X * screen_buf.dwSize.Y;
	// if (!FillConsoleOutputCharacterA(console_handle, ' ', num_chars, origin, &count))
	// 	return SABR_OPERR_WHAT;
	// if (!FillConsoleOutputCharacterA(console_handle, screen_buf.wAttributes, num_chars, origin, &count))
	// 	return SABR_OPERR_WHAT;
	// fputs("\e[2J", (FILE*) file.p);
	// return sabr_interpreter_set_cursor_pos_x((FILE*) file.p, 0);
#else
#endif
	fputs("\e[2J", (FILE*) file.p);
	return SABR_OPERR_NONE;
}

const uint32_t sabr_bif_func(io, File__toggle_cursor)(sabr_interpreter_t* inter) {
	sabr_value_t flag, file;
	if (!sabr_interpreter_pop(inter, &file)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &flag)) return SABR_OPERR_STACK;
#ifdef _WIN32
	// HANDLE console_handle = sabr_interpreter_get_console_handle((FILE*) file.p);
	// CONSOLE_CURSOR_INFO cursor_info;
	// if (!GetConsoleCursorInfo(console_handle, &cursor_info)) return SABR_OPERR_WHAT;
	// cursor_info.bVisible = flag.u != 0;
	// if (!SetConsoleCursorInfo(console_handle, &cursor_info)) return SABR_OPERR_WHAT;
#else
#endif
	fputs(flag.u ? "\e[?25h" : "\e[?25l", (FILE*) file.p);
	return SABR_OPERR_NONE;
}

sabr_bif_func_t sabr_bif_io_functions[] = {
	sabr_bif_func(io, std_out),
	sabr_bif_func(io, File__putc),
	sabr_bif_func(io, File__puts),
	sabr_bif_func(io, File__set_cursor_pos),
	sabr_bif_func(io, File__clear_screen),
	sabr_bif_func(io, File__toggle_cursor)
};
size_t sabr_bif_io_functions_len = sizeof(sabr_bif_io_functions) / sizeof(sabr_bif_func_t);

bool sabr_interpreter_fputc(sabr_interpreter_t* inter, sabr_value_t character, sabr_value_t file) {
	if (character.u < 127) fputc(character.u, (FILE*) file.p);
	else {
		char out[8];
		size_t rc = c32rtomb(out, (char32_t) character.u, &(inter->convert_state));
		if (rc == -1) return false;
		out[rc] = 0;
		fputs(out, (FILE*) file.p);
	}
	return true;
}

bool sabr_interpreter_fputs(sabr_interpreter_t* inter, sabr_value_t addr, sabr_value_t file) {
	sabr_value_t character;
	while (*addr.p) {
		character.u = *addr.p;
		if (!sabr_interpreter_fputc(inter, character, file)) return false;
		addr.p++;
	}
	return true;
}

const uint32_t sabr_interpreter_set_cursor_pos_x(FILE* file, int x) {
#ifdef _WIN32
	// HANDLE h = sabr_interpreter_get_console_handle(file);
	// CONSOLE_SCREEN_BUFFER_INFO screen_buf;
	// COORD origin;
	// if (!GetConsoleScreenBufferInfo(h, &screen_buf)) return SABR_OPERR_WHAT;
	// origin = screen_buf.dwCursorPosition;
	// origin.X = (SHORT) x;
	// if (!SetConsoleCursorPosition(h, origin)) return SABR_OPERR_WHAT;
#else
#endif
	fprintf(file, "\e[%dG", x + 1);
	return SABR_OPERR_NONE;
}

#ifdef _WIN32
HANDLE sabr_interpreter_get_console_handle(FILE* file) {
	return GetStdHandle(file == stderr ? STD_ERROR_HANDLE : STD_OUTPUT_HANDLE);
}
#endif