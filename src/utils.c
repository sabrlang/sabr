#include "utils.h"

#if defined (_WIN32)
	bool sabr_convert_string_mbr2c16(const char* src, wchar_t* dest, mbstate_t* convert_state) {
		const char* end = src + strlen(src);

		while (true) {
			char16_t out;
			size_t rc = mbrtoc16(&out, src, end - src + 1, convert_state);
			if (!rc) return true;
			if (rc == (size_t) -3) dest++;
			if (rc > (size_t) -3) return false;

			src += rc; *(dest++) = out;
		}
	}
	bool sabr_convert_string_c16rtomb(const wchar_t* src, char* dest, mbstate_t* convert_state) {
		size_t src_len = wcslen(src);
		char *p = dest;
		for (size_t n = 0; n < src_len; ++n) {
			size_t rc = c16rtomb(p, src[n], convert_state);
			if (rc == (size_t) -1) return false;
			p += rc;
		}
		return true;
	}
#endif

#if defined (_WIN32)
	bool sabr_get_full_path(const char* src_mbr, char* dest_mbr, wchar_t* dest_utf16, mbstate_t* convert_state) {
		wchar_t src_utf16[PATH_MAX] = {0, };

		if (!sabr_convert_string_mbr2c16(src_mbr, src_utf16, convert_state)) return false;
		if (!_wfullpath(dest_utf16, src_utf16, PATH_MAX)) return false;
		if (!_fullpath(dest_mbr, src_mbr, PATH_MAX)) return false;
		return true;
	}

	bool sabr_get_executable_path(char* dest, mbstate_t* convert_state) {
		wchar_t binary_path_windows[PATH_MAX];
		if (!GetModuleFileNameW(NULL, binary_path_windows, PATH_MAX)) return false;
		if (!sabr_convert_string_c16rtomb(binary_path_windows, dest, convert_state)) return false;
		return true;
	}

	bool sabr_get_std_lib_path(const char* lib_filename, char* dest, bool with_ext, mbstate_t* convert_state) {
		char drive[_MAX_DRIVE];
		char pivot_dir[_MAX_DIR];
		char binary_path[PATH_MAX];
		if (!sabr_get_executable_path(binary_path, convert_state)) return false;
		_splitpath(binary_path, drive, pivot_dir, NULL, NULL);
		strcat(pivot_dir, "..\\lib");
		_makepath(dest, drive, pivot_dir, lib_filename, with_ext ? ".sabrc" : NULL);
		return true;
	}
#else
	bool sabr_get_full_path(const char* src, char* dest) {
		if (!realpath(src, dest)) return false;
		return true;
	}

	bool sabr_get_executable_path(char* dest) {
		return readlink("/proc/self/exe", dest, PATH_MAX) >= 0;
	}

	bool sabr_get_std_lib_path(const char* lib_filename, char* dest, bool with_ext) {
		char binary_path[PATH_MAX] = {0, };
		char temp_path_filename[PATH_MAX] = {0, };
		char temp_path_dirname[PATH_MAX] = {0, };
		char* pivot_dir;
		if (!sabr_get_executable_path(binary_path)) return false;
		strcpy(temp_path_filename, binary_path);
		pivot_dir = dirname(temp_path_filename);
		strcpy(temp_path_dirname, pivot_dir);
		strcat(temp_path_dirname, "/../lib/");

		strcpy(dest, temp_path_dirname);
		strcat(dest, lib_filename);
		if (with_ext) strcat(dest, ".sabrc");

		return true;
	}
#endif