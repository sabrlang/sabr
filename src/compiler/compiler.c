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
	vector(token)* preprocessed_tokens;

	if (!sabr_compiler_load_file(comp, filename, &textcode_index)) {
		return false;
	}

	preprocessed_tokens = sabr_compiler_preprocess_textcode(comp, textcode_index);

	return true;
}
bool sabr_compiler_load_file(sabr_compiler* const comp, const char* filename, size_t* index) {
	FILE* file;
	char filename_full[PATH_MAX] = {0, };

#if defined(_WIN32)
	wchar_t filename_windows[PATH_MAX] = {0, };
	wchar_t filename_full_windows[PATH_MAX] = {0, };

	const char* u8_cvt_iter = filename;
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

vector(token)* sabr_compiler_preprocess_textcode(sabr_compiler* const comp, size_t textcode_index) {
	token* tokens;
	const char* code = *vector_at(cctl_ptr(char), &comp->textcode_vector, textcode_index);
	pos init_pos = {line: 1, column: 1};
	tokens = sabr_compiler_tokenize_string(comp, code, textcode_index, init_pos, false);

	return tokens;
}
vector(token)* sabr_compiler_tokenize_string(sabr_compiler* const comp, const char* textcode, size_t textcode_index, pos init_pos, bool is_generated) {
	size_t current_index = 0;
	size_t begin_index = 0;
	size_t end_index = 0;

	pos current_pos = init_pos;
	pos begin_pos = init_pos;
	pos end_pos = {0, };

	size_t brace_level;

	comment_parse_mode comment = CMNT_PARSE_NONE;
	string_parse_mode string_parse = STR_PARSE_NONE;
	bool string_escape = false;
	bool space = true;
	
	char character = *(textcode + current_index);
	
	vector(token)* tokens = (vector(token)*) malloc(sizeof(vector(token)));

	if (!tokens) {
		return NULL;
	}

	vector_init(token, tokens);

	while (character) {
		switch (character) {
			case '\n': 
				
			case '\r':
				if (comment == CMNT_PARSE_LINE) {
					space = true;
					comment = CMNT_PARSE_NONE;
				}
			case '\t':
			case ' ': {
				if (!comment) {
					if (!space) {
						if (string_parse) {
							if (string_escape) string_escape = false;
						}
						else {
							token t;

							end_index = current_index;
							end_pos = current_pos;

							t.data = new_string_slice(textcode, begin_index, end_index);
							if (!t.data) {
								goto FREE_ALL;
							}

							t.is_generated = is_generated;
							if (!is_generated) {
								t.begin_index = begin_index;
								t.end_index = end_index;
								t.begin_pos = begin_pos;
								t.end_pos = end_pos;
								t.textcode_index = textcode_index;
							}

							printf("%s\n", t.data);

							if (!vector_push_back(token, tokens, t)) {
								goto FREE_ALL;
							}

							space = true;
						}
					}
				}
			} break;
			case '\'': {
				if (!comment) {
					if (string_parse) {
						if (string_escape) string_escape = false;
						else {
							if (string_parse == STR_PARSE_SINGLE) {
								string_parse = STR_PARSE_NONE;
							}
						}
					}
					else if (space) {
						space = false;
						begin_index = current_index;
						begin_pos = current_pos;
						string_parse = STR_PARSE_SINGLE;
						string_escape = false;
					}
				}
			} break;
			case '\"': {
				if (!comment) {
					if (string_parse) {
						if (string_escape) string_escape = false;
						else {
							if (string_parse == STR_PARSE_DOUBLE) {
								string_parse = STR_PARSE_NONE;
							}
						}
					}
					else if (space) {
						space = false;
						begin_index = current_index;
						begin_pos = current_pos;
						string_parse = STR_PARSE_DOUBLE;
						string_escape = false;
					}
				}
			} break;
			case '{': {
				if (!comment) {
					if (string_parse) {
						if (string_escape) string_escape = false;
						else {
							if (string_parse == STR_PARSE_PREPROC) {
								brace_level++;
							}
						}
					}
					else if (space) {
						space = false;
						begin_index = current_index;
						begin_pos = current_pos;
						string_parse = STR_PARSE_PREPROC;
						string_escape = false;
						brace_level++;
					}
				}
			} break;
			case '}': {
				if (!comment) {
					if (string_parse) {
						if (string_escape) string_escape = false;
						else {
							if (string_parse == STR_PARSE_PREPROC) {
								brace_level--;
								if (brace_level == 0) {
									string_parse = STR_PARSE_NONE;
								}
							}
						}
					}
					else if (space) {
						space = false;
						begin_index = current_index;
						begin_pos = current_pos;
					}
				}
			} break;
			case '\\': {
				if (!comment) {
					if (string_parse) {
						if (string_escape) string_escape = false;
						else string_escape = true;
					}
					else if (space) {
						space = false;
						comment = CMNT_PARSE_LINE;
					}
				}
			} break;
			case '(': {
				if (!comment) {
					if (string_parse) {
						if (string_escape) string_escape = false;
					}
					else if (space) {
						space = false;
						comment = CMNT_PARSE_STACK;
					}
				}
			} break;
			case ')': {
				if (!comment) {
					if (string_parse) {
						if (string_escape) string_escape = false;
					}
				}
				if (comment == CMNT_PARSE_STACK) {
					space = true;
					comment = CMNT_PARSE_NONE;
				}
			} break;
			default: {
				if (!comment) {
					if (string_parse) {
						if (string_escape) {
							string_escape = false;
						}
					}
					if (space) {
						space = false;
						begin_index = current_index;
						begin_pos = current_pos;
					}
				}
			}
		}
		current_index++;
		character = *(textcode + current_index);
		
		if (((signed char) character) >= -64) {
			current_pos.column++;
		}
		
	}

	return tokens;
FREE_ALL:
	free(tokens);
	return NULL;
}