#include "compiler.h"
#include "preproc_operation.h"

bool sabr_compiler_init(sabr_compiler* const comp) {
	setlocale(LC_ALL, "en_US.utf8");

	trie_init(size_t, &comp->filename_trie);

	vector_init(cctl_ptr(char), &comp->filename_vector);
	vector_init(cctl_ptr(char), &comp->textcode_vector);

	trie_init(word, &comp->preproc_dictionary);

	for (size_t i = 0; i < preproc_keyword_names_len; i++) {
		word w;
		w.type = WT_PREPROC_KWRD;
		w.data.p_kwrd = (preproc_keyword) i;
		if (!trie_insert(word, &comp->preproc_dictionary, preproc_keyword_names[i], w)) {
			fputs(sabr_errmsg_alloc, stderr);
			return false;
		}
	}

	return true;
}

bool sabr_compiler_del(sabr_compiler* const comp) {

	trie_free(size_t, &comp->filename_trie);

	for (size_t i = 0; i < comp->filename_vector.size; i++)
		free(*vector_at(cctl_ptr, &comp->filename_vector, i));
	vector_free(cctl_ptr(char), &comp->filename_vector);

	for (size_t i = 0; i < comp->textcode_vector.size; i++)
		free(*vector_at(cctl_ptr, &comp->textcode_vector, i));
	vector_free(cctl_ptr(char), &comp->textcode_vector);

	sabr_free_word_trie(&comp->preproc_dictionary);

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
		if (rc > (size_t) -3) {
			fputs(sabr_errmsg_fullpath, stderr);
			goto FREE_ALL;
		}

		u8_cvt_iter += rc;
		*store_iter = out;
		store_iter++;
	}

	if (!_wfullpath(filename_full_windows, filename_windows, PATH_MAX)) {
		fputs(sabr_errmsg_fullpath, stderr);
		goto FREE_ALL;
	}
	if (!_fullpath(filename_full, filename, PATH_MAX)) {
		fputs(sabr_errmsg_fullpath, stderr);
		goto FREE_ALL;
	}

	if (_waccess(filename_full_windows, R_OK)) {
		fputs(sabr_errmsg_open, stderr);
		goto FREE_ALL;
	}

	file = _wfopen(filename_full_windows, L"rb");
#else
	if (!(realpath(filename, filename_full))) goto FAILURE_FILEPATH;

	if (access(fname, R_OK)) {
		fputs(sabr_errmsg_open, stderr);
		goto FREE_ALL;
	}

	file = fopen(filename_full, "rb");
#endif

	if (!file) {
		fputs(sabr_errmsg_open, stderr);
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
		fputs(sabr_errmsg_alloc, stderr);
		goto FREE_ALL;
	}
	
	int read_result = fread(textcode, size, 1, file);
	if (read_result != 1) {
		fclose(file);
		fputs(sabr_errmsg_read, stderr);
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
		fputs(sabr_errmsg_alloc, stderr);
		goto FREE_ALL;
	}

	if (!vector_push_back(cctl_ptr(char), &comp->filename_vector, filename_full_new)) {
		fputs(sabr_errmsg_alloc, stderr);
		goto FREE_ALL;
	}

	if (!vector_push_back(cctl_ptr(char), &comp->textcode_vector, textcode)) {
		fputs(sabr_errmsg_alloc, stderr);
		goto FREE_ALL;
	}

	return true;

FREE_ALL:
	free(textcode);
	free(filename_full_new);
	return false;
}

vector(token)* sabr_compiler_preprocess_textcode(sabr_compiler* const comp, size_t textcode_index) {
	vector(token)* tokens;
	const char* code = *vector_at(cctl_ptr(char), &comp->textcode_vector, textcode_index);
	pos init_pos = {line: 1, column: 1};
	tokens = sabr_compiler_tokenize_string(comp, code, textcode_index, init_pos, false);

	if (!tokens) {
		fputs(sabr_errmsg_tokenize, stderr);
	}
	for (size_t i = 0; i < tokens->size; i++) {
		token t = *vector_at(token, tokens, i);
	}

	tokens = sabr_compiler_preprocess_tokens(comp, tokens);
	if (!tokens) {
		fputs(sabr_errmsg_preprocess, stderr);
		return NULL;
	}

	for (size_t i = 0; i < tokens->size; i++) {
		token t = *vector_at(token, tokens, i);
		printf("token : %s\n", t.data);
	}

	return tokens;
}

vector(token)* sabr_compiler_preprocess_tokens(sabr_compiler* const comp, vector(token)* input_tokens) {
	vector(token)* output_tokens;

	output_tokens = (vector(token)*) malloc(sizeof(vector(token)));
	if (!output_tokens) {
		fputs(sabr_errmsg_alloc, stderr);
		goto FREE_ALL;
	}
	vector_init(token, output_tokens);

	for (size_t i = 0; i < input_tokens->size; i++) {
		token t = *vector_at(token, input_tokens, i);
		word* w = trie_find(word, &comp->preproc_dictionary, t.data);
		if (w) {
			switch (w->type) {
				case WT_PREPROC_KWRD: {
					if (!preproc_keyword_functions[w->data.p_kwrd](comp, *w, t, output_tokens)) {
						goto FREE_ALL;
					}
				} break;
				case WT_PREPROC_IDFR: {
					vector(token)* evaled_tokens;
					evaled_tokens = sabr_compiler_preprocess_eval_token(comp, w->data.macro_code);
					if (!evaled_tokens) {
						goto FREE_ALL;
					}
					for (size_t i = 0; i < evaled_tokens->size; i++) {
						token evaled_token = *vector_at(token, evaled_tokens, i);
						if (!vector_push_back(token, output_tokens, evaled_token)) {
							fputs(sabr_errmsg_alloc, stderr);
							goto FREE_ALL;
						}
					}
					free(evaled_tokens);
				} break;
				default:
					break;
			}
		}
		else {
			t.data = sabr_new_string_copy(t.data);
			if (!vector_push_back(token, output_tokens, t)) {
				fputs(sabr_errmsg_alloc, stderr);
				goto FREE_ALL;
			}
		}
	}

	sabr_free_token_vector(input_tokens);
	return output_tokens;

FREE_ALL:
	sabr_free_token_vector(input_tokens);
	sabr_free_token_vector(output_tokens);
	return NULL;
}

vector(token)* sabr_compiler_preprocess_eval_token(sabr_compiler* const comp, token t) {
	vector(token)* input_tokens = NULL;
	vector(token)* output_tokens = NULL;
	char* temp_input_string = NULL;
	char* input_string = NULL;
	pos input_pos = t.begin_pos;
	size_t input_index = t.textcode_index;

	if (t.data[0] == '{') {
		temp_input_string = sabr_new_string_slice(t.data, 1, strlen(t.data) - 1);
		input_pos.column++;
	}
	else temp_input_string = sabr_new_string_copy(t.data);

	input_string = sabr_new_string_append(temp_input_string, " \n\0");

	input_tokens = sabr_compiler_tokenize_string(comp, input_string, input_index, input_pos, t.is_generated);
	if (!input_tokens) {
		fputs(sabr_errmsg_tokenize, stderr);
		goto FREE_ALL;
	}

	output_tokens = sabr_compiler_preprocess_tokens(comp, input_tokens);
	if (!output_tokens) {
		fputs(sabr_errmsg_preprocess, stderr);
		goto FREE_ALL;
	}
	free(input_string);
	free(temp_input_string);

	return output_tokens;

FREE_ALL:
	free(input_string);
	free(temp_input_string);
	sabr_free_token_vector(input_tokens);
	sabr_free_token_vector(output_tokens);

	return NULL;
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
		fputs(sabr_errmsg_alloc, stderr);
		return NULL;
	}

	vector_init(token, tokens);

	while (character) {
		switch (character) {
			case '\n': 
				current_pos.line++;
				current_pos.column = 0;
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

							t.data = sabr_new_string_slice(textcode, begin_index, end_index);
							if (!t.data) {
								fputs(sabr_errmsg_alloc, stderr);
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


							if (!vector_push_back(token, tokens, t)) {
								fputs(sabr_errmsg_alloc, stderr);
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
						else if (string_parse == STR_PARSE_SINGLE) {
							string_parse = STR_PARSE_NONE;
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
						else if (string_parse == STR_PARSE_DOUBLE) {
							string_parse = STR_PARSE_NONE;
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
						else if (string_parse == STR_PARSE_PREPROC) {
							brace_level++;
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
						else if (string_parse == STR_PARSE_PREPROC) {
							brace_level--;
							if (brace_level == 0) {
								string_parse = STR_PARSE_NONE;
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
						string_escape = !string_escape;
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
	sabr_free_token_vector(tokens);
	return NULL;
}