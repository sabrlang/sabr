#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <uchar.h>

#if defined(_WIN32)
	#include <windows.h>
#else
	#include <libgen.h>
	#include <unistd.h>
	#if defined(__linux__)
		#include <linux/limits.h>
	#endif
#endif

#include "value.h"
#include "cctl_define.h"

#if defined (_WIN32)
    bool sabr_convert_string_mbr2c16(const char* src, wchar_t* dest, mbstate_t* convert_state);
    bool sabr_get_full_path(const char* src_mbr, char* dest_mbr, wchar_t* dest_utf16, mbstate_t* convert_state);
    bool sabr_get_executable_path(char* dest, mbstate_t* convert_state);
    bool sabr_get_std_lib_path(char* dest, const char* lib_filename, bool with_ext, mbstate_t* convert_state);
    void sabr_get_local_file_path(char* dest, const char* current_filename, const char* filename, bool with_ext, mbstate_t* convert_state);
#else
    bool sabr_get_full_path(const char* src, char* dest);
    bool sabr_get_executable_path(char* dest);
    bool sabr_get_std_lib_path(char* dest, const char* lib_filename, bool with_ext);
    void sabr_get_local_file_path(char* dest, const char* current_filename, const char* filename, bool with_ext);
#endif

bool sabr_convert_vstr_to_cvec(sabr_value_t src, vector(char)* dest, mbstate_t* convert_state);

#endif