#include "compiler.h"

bool sabr_compiler_init(sabr_compiler* const comp) {
	setlocale(LC_ALL, "en_US.utf8");

	trie_init(size_t, &comp->filename_trie);

	vector_init(cctl_ptr(char), &comp->filename_vector);
	vector_init(cctl_ptr(char), &comp->textcode_vector);

	return true;
}
bool sabr_compiler_del(sabr_compiler* const comp) {

	return true;
}

bool sabr_compiler_compile_file(sabr_compiler* const comp, const char* filename) {
	size_t textcode_index;

	if (!sabr_compiler_load_file(comp, filename, &textcode_index)) {
		return false;
	}
}
bool sabr_compiler_load_file(sabr_compiler* const comp, const char* filename, size_t* index) {
	FILE* file;
	char filename_full[PATH_MAX] = {0, };

#if defined(_WIN32)
	wchar_t filename_windows[PATH_MAX] = {0, };
	wchar_t filename_full_windows[PATH_MAX] = {0, };

	char* u8_cvt_iter = filename;
	wchar_t* store_iter = filename_windows;
	const char* end = filename + strlen(filename);

	while (true) {
		char16_t out;
		size_t rc = mbrtoc16(
			&out,
			u8_cvt_iter,
			end - u8_cvt_iter + 1,
			&(comp->convert_state)
		);
		if (!rc) break;
		if (rc == (size_t) -3) {
			store_iter++;
		}
		if (rc > (size_t) -3) goto FAILURE_FILEPATH;

		u8_cvt_iter += rc;
		*store_iter = out;
		store_iter++;
	}

	if (!_wfullpath(filename_full_windows, filename_windows, PATH_MAX)) goto FAILURE_FILEPATH;
	if (!_fullpath(filename_full, filename, PATH_MAX)) goto FAILURE_FILEPATH;
#else
	if (!(realpath(filename, filename_full))) goto FAILURE_FILEPATH;
#endif

#if defined(_WIN32)
	file = _wfopen(filename_full_windows, L"rb");
#else
	file = fopen(filename_full, "rb");
#endif

#if defined(_WIN32)
	file = _wfopen(filename_full_windows, L"rb");
#else
	file = fopen(filename_full, "rb");
#endif

FAILURE_FILEPATH:
	return false;
}

bool sabr_compiler_preprocess_textcode(sabr_compiler* const comp, size_t textcode_index, vector(token)** output_tokens) {

}
vector(token)* sabr_compiler_tokenize_string(sabr_compiler* const comp, const char* textcode, size_t textcode_index, pos init_pos, bool is_generated) {
	
}