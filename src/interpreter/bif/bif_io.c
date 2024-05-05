#include "bif_io.h"

const uint32_t sabr_bif_func(io, fputc)(sabr_interpreter_t* inter) {
	sabr_value_t file, character;
	if (!sabr_interpreter_pop(inter, &file)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &character)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_fputc(inter, character, file)) return SABR_OPERR_UNICODE;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_bif_func(io, fputs)(sabr_interpreter_t* inter) {
	sabr_value_t file, addr;
	if (!sabr_interpreter_pop(inter, &file)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &addr)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_fputs(inter, addr, file)) return SABR_OPERR_UNICODE;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_bif_func(io, gotoxy)(sabr_interpreter_t* inter) {
	sabr_value_t x, y;
	if (!sabr_interpreter_pop(inter, &y)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &x)) return SABR_OPERR_STACK;
#ifdef _WIN32
	COORD pos = { x.u, y.u };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
#else
	printf("%c[%d;%df", 0x1B, y, x);
#endif
	return SABR_OPERR_NONE;
}

const uint32_t sabr_bif_func(io, cls)(sabr_interpreter_t* inter) {
#ifdef _WIN32
	system("cls");
#else
	system("clear");
#endif
	return SABR_OPERR_NONE;
}

const uint32_t sabr_bif_func(io, toggle_cursor)(sabr_interpreter_t* inter) {
	sabr_value_t flag;
	if (!sabr_interpreter_pop(inter, &flag)) return SABR_OPERR_STACK;
#ifdef _WIN32
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO info;
	info.dwSize = 100;
	info.bVisible = flag.u != 0;
	SetConsoleCursorInfo(consoleHandle, &info);
#else
	if (flag.u) printf("\e[?25h");
	else printf("\e[?25l");
#endif
	return SABR_OPERR_NONE;
}

sabr_bif_func_t sabr_bif_io_functions[] = {
	sabr_bif_func(io, fputc),
	sabr_bif_func(io, fputs),
	sabr_bif_func(io, gotoxy),
	sabr_bif_func(io, cls),
	sabr_bif_func(io, toggle_cursor)
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
		character.u = *(addr.p);
		if (!sabr_interpreter_fputc(inter, character, file)) return false;
		addr.p++;
	}
	return true;
}