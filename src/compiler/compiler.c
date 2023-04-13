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
		size_t rc = mbrtoc16(&out, u8_cvt_iter, end - u8_cvt_iter + 1, &(comp->convert_state));
		if (!rc) break;
		if (rc == (size_t) -3) store_iter++;
		if (rc > (size_t) -3) goto FAILURE_FILEPATH;

		u8_cvt_iter += rc;
		*store_iter = out;
		store_iter++;
	}

	if (!_wfullpath(filename_full_windows, filename_windows, PATH_MAX)) goto FAILURE_FILEPATH;
	if (!_fullpath(filename_full, filename, PATH_MAX)) goto FAILURE_FILEPATH;

	if (_waccess(filename_full_windows, R_OK)) {
		goto FREE_ALL;
	}

	file = _wfopen(filename_full_windows, L"rb");
#else
	if (!(realpath(filename, filename_full))) goto FAILURE_FILEPATH;

	if (access(fname, R_OK)) {
		goto FREE_ALL;
	}

	file = fopen(filename_full, "rb");
#endif

	if (!file) {
		goto FREE_ALL;
	}

	fseek(file, 0, SEEK_END);
	size_t size = ftell(file);
	rewind(file);

	char* textcode = (char*) malloc(size + 3);
	
	int filename_size = strlen(filename_full) + 1;
	char* filename_full_new = (char*) malloc(filename_size);

	if (!(textcode && filename_full_new)) {
		fclose(file);
		goto FREE_ALL;
	}
	
	int read_result = fread(textcode, size, 1, file);
	if (read_result != 1) {
		fclose(file);
		goto FREE_ALL;
	}

	fclose(file);

	textcode[size] = ' ';
	textcode[size + 1] = '\n';
	textcode[size + 2] = '\0';

	#if defined(_WIN32)
		memcpy_s(filename_full_new, filename_size, filename_full, filename_size);
	#else
		memcpy(filename_full_new, filename_full, filename_size);
	#endif

	*index = comp->textcode_vector.size;
	
	if (!trie_insert(size_t, &comp->filename_trie, filename_full_new, *index)) {
		goto FREE_ALL;
	}

	if (!vector_push_back(cctl_ptr(char), &comp->filename_vector, filename_full_new)) {
		goto FREE_ALL;
	}

	if (!vector_push_back(cctl_ptr(char), &comp->textcode_vector, textcode)) {
		goto FREE_ALL;
	}

	return true;

FAILURE_FILEPATH:
	goto FREE_ALL;

FREE_ALL:
	return false;
}

bool sabr_compiler_preprocess_textcode(sabr_compiler* const comp, size_t textcode_index, vector(token)** output_tokens) {

}
vector(token)* sabr_compiler_tokenize_string(sabr_compiler* const comp, const char* textcode, size_t textcode_index, pos init_pos, bool is_generated) {
	
}