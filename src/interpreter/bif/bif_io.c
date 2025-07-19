#include "bif_io.h"

const uint32_t sabr_bif_func(io, std_out)(sabr_interpreter_t* inter, size_t* index) {
	sabr_value_t v_file;
	v_file.p = (uint64_t*) stdout;
	if (!sabr_interpreter_push(inter, v_file)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_bif_func(io, std_err)(sabr_interpreter_t* inter, size_t* index) {
	sabr_value_t v_file;
	v_file.p = (uint64_t*) stderr;
	if (!sabr_interpreter_push(inter, v_file)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_bif_func(io, std_in)(sabr_interpreter_t* inter, size_t* index) {
	sabr_value_t v_file;
	v_file.p = (uint64_t*) stderr;
	if (!sabr_interpreter_push(inter, v_file)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}


const uint32_t sabr_bif_func(io, File__putc)(sabr_interpreter_t* inter, size_t* index) {
	sabr_value_t v_file, v_character;
	if (!sabr_interpreter_pop(inter, &v_file)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &v_character)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_fputc(inter, v_character, v_file)) return SABR_OPERR_UNICODE;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_bif_func(io, File__puts)(sabr_interpreter_t* inter, size_t* index) {
	sabr_value_t v_file, v_addr;
	if (!sabr_interpreter_pop(inter, &v_file)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &v_addr)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_fputs(inter, v_addr, v_file)) return SABR_OPERR_UNICODE;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_bif_func(io, File__printf)(sabr_interpreter_t* inter, size_t* index) {
	sabr_value_t v_file, v_format;
	if (!sabr_interpreter_pop(inter, &v_file)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &v_format)) return SABR_OPERR_STACK;
	sabr_value_t v_character, v_arg;

	while (*v_format.p) {
		v_character.u = *v_format.p;
		if (v_character.u == '%') {
			char format_buffer[256] = "%";
			size_t format_buffer_index = 1;

			v_format.p++;
			v_character.u = *v_format.p;
			bool is_flag_field = true;
			while (is_flag_field) {
				switch (v_character.u) {
					case '-': case '+': case ' ': case '0': case '#':
						format_buffer[format_buffer_index++] = (char) v_character.u;
						v_format.p++;
						v_character.u = *v_format.p;
						break;
					default: is_flag_field = false;
				}
			}

			bool is_width_field = true;
			while (is_width_field) {
				switch (v_character.u) {
					case '0' ... '9':
						format_buffer[format_buffer_index++] = (char) v_character.u;
						v_format.p++;
						v_character.u = *v_format.p;
						break;
					default: is_width_field = false;
				}
			}

			bool is_precision_field = v_character.u == '.';
			if (is_precision_field) {
				format_buffer[format_buffer_index++] = (char) v_character.u;
				v_format.p++;
				v_character.u = *v_format.p;
			}
			while (is_precision_field) {
				switch (v_character.u) {
					case '0' ... '9':
						format_buffer[format_buffer_index++] = (char) v_character.u;
						v_format.p++;
						v_character.u = *v_format.p;
						break;
					default: is_precision_field = false;
				}
			}

			int length_field_count = 2;
			while (length_field_count > 0) {
				switch (v_character.u) {
					case 'h': case 'x': case 'l': case 'L': case 'z': case 'j': case 't':
						format_buffer[format_buffer_index++] = (char) v_character.u;
						v_format.p++;
						v_character.u = *v_format.p;
						length_field_count--;
						break;
					default: length_field_count = 0;
				}
			}

			switch (v_character.u) {
				case '%': case 'd': case 'i': case 'u':
				case 'f': case 'F': case 'e': case 'E':
				case 'g': case 'G': case 'x': case 'X':
				case 'o': case 'c': case 'p':
				case 'a': case 'A': case 'n':
					format_buffer[format_buffer_index++] = (char) v_character.u;
					if (!sabr_interpreter_pop(inter, &v_arg)) return SABR_OPERR_STACK;
					printf(format_buffer, v_arg.u);
					break;
				case 's': {
					vector(char) string_buffer;
					vector_init(char, &string_buffer);

					format_buffer[format_buffer_index++] = (char) v_character.u;
					if (!sabr_interpreter_pop(inter, &v_arg)) return SABR_OPERR_STACK;
					if (!sabr_convert_vstr_to_cvec(v_arg, &string_buffer, &inter->convert_state)) return SABR_OPERR_UNICODE;
					printf(format_buffer, string_buffer.p_data);

					vector_free(char, &string_buffer);
				} break;
				default:
			}
		}
		else {
			if (!sabr_interpreter_fputc(inter, v_character, v_file)) return SABR_OPERR_UNICODE;
		}
		v_format.p++;
	}

	return SABR_OPERR_NONE;
}

const uint32_t sabr_bif_func(io, File__open)(sabr_interpreter_t* inter, size_t* index) {
	uint32_t result = SABR_OPERR_NONE;

	sabr_value_t v_mode, v_filename, v_file;

	FILE* file = NULL;

	vector(char) filename_string_buffer;
	vector_init(char, &filename_string_buffer);

	vector(char) mode_string_buffer;
	vector_init(char, &mode_string_buffer);

	if (!sabr_interpreter_pop(inter, &v_filename)) { result = SABR_OPERR_STACK; goto RETURN_RESULT; }
	if (!sabr_interpreter_pop(inter, &v_mode)) { result = SABR_OPERR_STACK; goto RETURN_RESULT; }

	if (!sabr_convert_vstr_to_cvec(v_filename, &filename_string_buffer, &inter->convert_state)) { result = SABR_OPERR_UNICODE; goto RETURN_RESULT; }
	if (!sabr_convert_vstr_to_cvec(v_mode, &mode_string_buffer, &inter->convert_state)) { result = SABR_OPERR_UNICODE; goto RETURN_RESULT; }

#ifdef _WIN32
	wchar_t filename_windows[PATH_MAX] = {0, };
	if (!sabr_convert_string_mbr2c16(filename_string_buffer.p_data, filename_windows, &inter->convert_state)) { result = SABR_OPERR_UNICODE; goto RETURN_RESULT; }
	wchar_t mode_windows[4] = {0, };
	if (!sabr_convert_string_mbr2c16(mode_string_buffer.p_data, mode_windows, &inter->convert_state)) { result = SABR_OPERR_UNICODE; goto RETURN_RESULT; }

	file = _wfopen(filename_windows, mode_windows);
#else
	file = fopen(filename_string_buffer.p_data, mode_string_buffer.p_data);
#endif

RETURN_RESULT:
	if (result) {
		if (file) fclose(file);
		file = NULL;
	}

	v_file.p = (uint64_t*) file;

	if (!sabr_interpreter_push(inter, v_file)) return SABR_OPERR_STACK;

	vector_free(char, &filename_string_buffer);
	vector_free(char, &mode_string_buffer);
	return result;
}

const uint32_t sabr_bif_func(io, File__close)(sabr_interpreter_t* inter, size_t* index) {
	sabr_value_t v_file, v_result;
	if (!sabr_interpreter_pop(inter, &v_file)) return SABR_OPERR_STACK;
	v_result.u = fclose((FILE*) v_file.p);
	if (!sabr_interpreter_push(inter, v_result)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_bif_func(io, File__seek)(sabr_interpreter_t* inter, size_t* index) {
	sabr_value_t v_origin, v_offset, v_file, v_result;
	if (!sabr_interpreter_pop(inter, &v_file)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &v_offset)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &v_origin)) return SABR_OPERR_STACK;
	v_result.i = fseek((FILE*) v_file.p, v_offset.i, v_origin.i);
	if (!sabr_interpreter_push(inter, v_result)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_bif_func(io, File__tell)(sabr_interpreter_t* inter, size_t* index) {
	sabr_value_t v_file, v_result;
	if (!sabr_interpreter_pop(inter, &v_file)) return SABR_OPERR_STACK;
	v_result.i = ftell((FILE*) v_file.p);
	if (!sabr_interpreter_push(inter, v_result)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_bif_func(io, File__rewind)(sabr_interpreter_t* inter, size_t* index) {
	sabr_value_t v_file;
	if (!sabr_interpreter_pop(inter, &v_file)) return SABR_OPERR_STACK;
	rewind((FILE*) v_file.p);
	return SABR_OPERR_NONE;
}
const uint32_t sabr_bif_func(io, File__read_utf8)(sabr_interpreter_t* inter, size_t* index) {
	sabr_value_t v_bytes, v_dst, v_file, v_result;
	if (!sabr_interpreter_pop(inter, &v_file)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &v_dst)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &v_bytes)) return SABR_OPERR_STACK;

	char* buffer = (char*) malloc(v_bytes.u + 1);
	memset(buffer, 0, v_bytes.u + 1);
	if (!buffer) return SABR_OPERR_MEMORY;

	size_t read_result = fread(buffer, 1, v_bytes.u, (FILE*) v_file.p);
	v_result.u = read_result;
	char* buffer_end = buffer + read_result + 1;
	char* current_buffer = buffer;
	size_t rc;
	char32_t out;
	while ((rc = mbrtoc32(&out, current_buffer, buffer_end - current_buffer, &inter->convert_state))) {
		if (rc >= (size_t) -3) { free(buffer); return SABR_OPERR_UNICODE; }
		current_buffer += rc;
		*(v_dst.p++) = out;
	}
	if (!sabr_interpreter_push(inter, v_result)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_bif_func(io, File__read_bytes)(sabr_interpreter_t* inter, size_t* index) {
	sabr_value_t v_bytes, v_dst, v_file, v_result;
	if (!sabr_interpreter_pop(inter, &v_file)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &v_dst)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &v_bytes)) return SABR_OPERR_STACK;
	v_result.u = 0;
	for (size_t i = 0; i < v_bytes.u; i++) {
		sabr_value_t v_buffer = {0, };
		size_t current_result;
		current_result = fread(&v_buffer, sizeof(char), 1, (FILE*) v_file.p);
		if (current_result != 1) break;
		v_result.u += current_result;
		*(v_dst.p++) = v_buffer.u;
	}
	if (!sabr_interpreter_push(inter, v_result)) return SABR_OPERR_STACK;
	return SABR_OPERR_NONE;
}

const uint32_t sabr_bif_func(io, File__set_cursor_pos)(sabr_interpreter_t* inter, size_t* index) {
	sabr_value_t v_x, v_y, v_file;
	if (!sabr_interpreter_pop(inter, &v_file)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &v_y)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &v_x)) return SABR_OPERR_STACK;
#ifdef _WIN32
	// COORD pos = { v_x.u, v_y.u };
	// HANDLE console_handle = sabr_interpreter_get_console_handle((FILE*) file.p);
	// if (!SetConsoleCursorPosition(console_handle, pos)) return SABR_OPERR_WHAT;
#else
#endif
	fprintf((FILE*) v_file.p, "\e[%d;%df", (int) v_y.i, (int) v_x.i);
	return SABR_OPERR_NONE;
}

const uint32_t sabr_bif_func(io, File__clear_screen)(sabr_interpreter_t* inter, size_t* index) {
	sabr_value_t v_file;
	if (!sabr_interpreter_pop(inter, &v_file)) return SABR_OPERR_STACK;
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
	fputs("\e[2J", (FILE*) v_file.p);
	return SABR_OPERR_NONE;
}

const uint32_t sabr_bif_func(io, File__toggle_cursor)(sabr_interpreter_t* inter, size_t* index) {
	sabr_value_t v_flag, v_file;
	if (!sabr_interpreter_pop(inter, &v_file)) return SABR_OPERR_STACK;
	if (!sabr_interpreter_pop(inter, &v_flag)) return SABR_OPERR_STACK;
#ifdef _WIN32
	// HANDLE console_handle = sabr_interpreter_get_console_handle((FILE*) file.p);
	// CONSOLE_CURSOR_INFO cursor_info;
	// if (!GetConsoleCursorInfo(console_handle, &cursor_info)) return SABR_OPERR_WHAT;
	// cursor_info.bVisible = flag.u != 0;
	// if (!SetConsoleCursorInfo(console_handle, &cursor_info)) return SABR_OPERR_WHAT;
#else
#endif
	fputs(v_flag.u ? "\e[?25h" : "\e[?25l", (FILE*) v_file.p);
	return SABR_OPERR_NONE;
}

sabr_bif_func_t sabr_bif_io_functions[] = {
	sabr_bif_func(io, std_out),
	sabr_bif_func(io, std_err),
	sabr_bif_func(io, std_in),
	sabr_bif_func(io, File__putc),
	sabr_bif_func(io, File__puts),
	sabr_bif_func(io, File__printf),
	sabr_bif_func(io, File__open),
	sabr_bif_func(io, File__close),
	sabr_bif_func(io, File__seek),
	sabr_bif_func(io, File__tell),
	sabr_bif_func(io, File__rewind),
	sabr_bif_func(io, File__read_utf8),
	sabr_bif_func(io, File__read_bytes),
	sabr_bif_func(io, File__set_cursor_pos),
	sabr_bif_func(io, File__clear_screen),
	sabr_bif_func(io, File__toggle_cursor)
};
size_t sabr_bif_io_functions_len = sizeof(sabr_bif_io_functions) / sizeof(sabr_bif_func_t);

bool sabr_interpreter_fputc(sabr_interpreter_t* inter, sabr_value_t v_character, sabr_value_t v_file) {
	if (v_character.u < 127) fputc(v_character.u, (FILE*) v_file.p);
	else {
		char out[8];
		size_t rc = c32rtomb(out, (char32_t) v_character.u, &(inter->convert_state));
		if (rc == -1) return false;
		out[rc] = 0;
		fputs(out, (FILE*) v_file.p);
	}
	return true;
}

bool sabr_interpreter_fputs(sabr_interpreter_t* inter, sabr_value_t addr, sabr_value_t file) {
	sabr_value_t v_character;
	while (*addr.p) {
		v_character.u = *addr.p;
		if (!sabr_interpreter_fputc(inter, v_character, file)) return false;
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