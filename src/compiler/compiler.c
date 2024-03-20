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
		w.data.preproc_kwrd = (preproc_keyword) i;
		if (!trie_insert(word, &comp->preproc_dictionary, preproc_keyword_names[i], w)) {
			fputs(sabr_errmsg_alloc, stderr);
			return false;
		}
	}
	vector_init(cctl_ptr(trie(word)), &comp->preproc_local_dictionary_stack);
	vector_init(preproc_stop_flag, &comp->preproc_stop_stack);

	trie_init(word, &comp->dictionary);
	for (size_t i = 0; i < keyword_names_len; i++) {
		word w;
		w.type = WT_KWRD;
		w.data.kwrd = (keyword) i;
		if (!trie_insert(word, &comp->dictionary, keyword_names[i], w)) {
			fputs(sabr_errmsg_alloc, stderr);
			return false;
		}
	}
	for (size_t i = 0; i < bio_names_len; i++) {
		word w;
		w.type = WT_OP;
		w.data.oc = bio_indices[i];
		if (!trie_insert(word, &comp->dictionary, bio_names[i], w)) {
			fputs(sabr_errmsg_alloc, stderr);
			return false;
		}
	}
	comp->identifier_count = 0;

	vector_init(cctl_ptr(vector(keyword_data)), &comp->keyword_data_stack);

	comp->tab_size = 4;

	if (!memset(&comp->convert_state, 0, sizeof(mbstate_t))) return false;

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

	for (size_t i = 0; i < comp->preproc_local_dictionary_stack.size; i++)
		sabr_free_word_trie(*vector_at(cctl_ptr(trie(word)), &comp->preproc_local_dictionary_stack, i));
	vector_free(cctl_ptr(trie(word)), &comp->preproc_local_dictionary_stack);

	vector_free(preproc_stop_flag, &comp->preproc_stop_stack);

	sabr_free_word_trie(&comp->dictionary);

	for (size_t i = 0; i < comp->keyword_data_stack.size; i++)
		vector_free(keyword_data, *vector_at(cctl_ptr(vector(keyword_data)), &comp->keyword_data_stack, i));
	vector_free(cctl_ptr(vector(keyword_data)), &comp->keyword_data_stack);

	return true;
}

bytecode* sabr_compiler_compile_file(sabr_compiler* const comp, const char* filename) {
	size_t textcode_index;
	vector(token)* preprocessed_tokens = NULL;
	bytecode* compiled_bytecode = NULL;

	if (!sabr_compiler_load_file(comp, filename, &textcode_index)) {
		return NULL;
	}

	preprocessed_tokens = sabr_compiler_preprocess_textcode(comp, textcode_index);
	if (!preprocessed_tokens) return NULL;
	
	compiled_bytecode = sabr_compiler_compile_tokens(comp, preprocessed_tokens);
	if (!compiled_bytecode) {
		sabr_free_token_vector(preprocessed_tokens);
		free(preprocessed_tokens);
		return NULL;
	}

	return compiled_bytecode;
}
bool sabr_compiler_load_file(sabr_compiler* const comp, const char* filename, size_t* index) {
	FILE* file;
	char filename_full[PATH_MAX] = {0, };

	char* filename_full_new = NULL;
	int filename_size;
	char* textcode = NULL;

#if defined(_WIN32)
	wchar_t filename_full_windows[PATH_MAX] = {0, };

	if (!sabr_get_full_path(filename, filename_full, filename_full_windows, &(comp->convert_state))) {
		fputs(sabr_errmsg_fullpath, stderr);
		return false;
	}

	if (_waccess(filename_full_windows, R_OK)) {
		fputs(sabr_errmsg_open, stderr);
		return false;
	}

	file = _wfopen(filename_full_windows, L"rb");
#else
	if (!sabr_get_full_path(filename, filename_full)) {
		fputs(sabr_errmsg_fullpath, stderr);
		return false;
	}

	if (access(filename_full, R_OK)) {
		fputs(sabr_errmsg_open, stderr);
		return false;
	}

	file = fopen(filename_full, "rb");
#endif

	if (!file) {
		fputs(sabr_errmsg_open, stderr);
		return false;
	}

	fseek(file, 0, SEEK_END);
	size_t size = ftell(file);
	rewind(file);

	textcode = (char*) malloc(size + 3);
	
	filename_size = strlen(filename_full) + 1;
	filename_full_new = (char*) malloc(filename_size);

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

bool sabr_compiler_save_bytecode(sabr_compiler* const comp, bytecode* const bc, const char* filename) {
	FILE* file;
	char filename_full[PATH_MAX] = {0, };
	
#if defined(_WIN32)
	wchar_t filename_full_windows[PATH_MAX] = {0, };

	if (!sabr_get_full_path(filename, filename_full, filename_full_windows, &(comp->convert_state))) {
		fputs(sabr_errmsg_fullpath, stderr);
		return false;
	}
	file = _wfopen(filename_full_windows, L"wb");
#else
	if (!sabr_get_full_path(filename, filename_full)) {
		fputs(sabr_errmsg_fullpath, stderr);
		return false;
	}
	file = fopen(filename_full, "wb");
#endif

	if (!file) {
		fputs(sabr_errmsg_open, stderr);
		return false;
	}

	for (size_t i = 0; i < bc->bcop_vec.size; i++) {
		bytecode_operation bcop = *vector_at(bytecode_operation, &bc->bcop_vec, i);
		fputc(bcop.oc, file);
		if (sabr_opcode_has_operand(bcop.oc)) {
			for (size_t j = 0; j < 8; j++) {
				fputc((bcop.operand.bytes[j]), file);
			}
		}
	}

	fclose(file);
	return true;
}

vector(token)* sabr_compiler_preprocess_textcode(sabr_compiler* const comp, size_t textcode_index) {
	bool result = false;
	
	vector(token)* input_tokens = NULL;
	vector(token)* output_tokens = NULL;
	trie(word)* preproc_local_dictionary = NULL;
	const char* code = *vector_at(cctl_ptr(char), &comp->textcode_vector, textcode_index);
	pos init_pos = { .line = 1, .column = 1 };

	input_tokens = sabr_compiler_tokenize_string(comp, code, textcode_index, init_pos, false);

	if (!input_tokens) {
		fputs(sabr_errmsg_tokenize, stderr);
		goto FREE_ALL;
	}

	preproc_local_dictionary = (trie(word)*) malloc(sizeof(trie(word)));
	if (!preproc_local_dictionary) {
		fputs(sabr_errmsg_alloc, stderr);
		goto FREE_ALL;
	}
	trie_init(word, preproc_local_dictionary);
	if (!vector_push_back(cctl_ptr(trie(word)), &comp->preproc_local_dictionary_stack, preproc_local_dictionary)) {
		fputs(sabr_errmsg_alloc, stderr);
		goto FREE_ALL;
	}

	if (!vector_push_back(preproc_stop_flag, &comp->preproc_stop_stack, PPS_NONE)) {
		fputs(sabr_errmsg_alloc, stderr);
		goto FREE_ALL;
	}

	output_tokens = sabr_compiler_preprocess_tokens(comp, input_tokens, NULL);
	if (!output_tokens) {
		fputs(sabr_errmsg_preprocess, stderr);
		goto FREE_ALL;
	}

	result = true;
FREE_ALL:
	if (!result) {
		if (output_tokens) {
			sabr_free_token_vector(output_tokens);
			free(output_tokens);
			output_tokens = NULL;
		}
	}

	if (input_tokens) {
		sabr_free_token_vector(input_tokens);
		free(input_tokens);
		input_tokens = NULL;
	}

	sabr_free_word_trie(preproc_local_dictionary);
	free(preproc_local_dictionary);
	vector_pop_back(cctl_ptr(trie(word)), &comp->preproc_local_dictionary_stack);
	vector_pop_back(preproc_stop_flag, &comp->preproc_stop_stack);

	return output_tokens;
}

vector(token)* sabr_compiler_preprocess_tokens(sabr_compiler* const comp, vector(token)* input_tokens, vector(token)* output_tokens) {
	char* new_t_data = NULL;
	bool preproc_stop = false;

	if (!output_tokens) {
		output_tokens = (vector(token)*) malloc(sizeof(vector(token)));
		if (!output_tokens) {
			fputs(sabr_errmsg_alloc, stderr);
			goto FREE_ALL;
		}
		vector_init(token, output_tokens);
	}

	trie(word)* preproc_local_dictionary = *vector_back(cctl_ptr(trie(word)), &comp->preproc_local_dictionary_stack);

	for (size_t i = 0; i < input_tokens->size; i++) {
		token t = *vector_at(token, input_tokens, i);
		word* w = NULL;
		w = trie_find(word, preproc_local_dictionary, t.data);
		if (!w) w = trie_find(word, &comp->preproc_dictionary, t.data);
		if (w) {
			switch (w->type) {
				case WT_PREPROC_KWRD: {
					if (!preproc_keyword_functions[w->data.preproc_kwrd](comp, *w, t, output_tokens)) {
						fprintf(stderr, console_yellow console_bold "%s" console_reset " in line %zu, column %zu\n", t.data, t.begin_pos.line, t.begin_pos.column);
						fprintf(stderr, "in file " console_yellow console_bold "%s\n" console_reset, *vector_at(cctl_ptr(char), &comp->filename_vector, t.textcode_index));
						goto FREE_ALL;
					}
					preproc_stop_flag* current_preproc_stop = vector_back(preproc_stop_flag, &comp->preproc_stop_stack);
					if (*current_preproc_stop == PPS_BREAK || *current_preproc_stop == PPS_CONTINUE) {
						preproc_stop = true;
						break;
					}
				} break;
				case WT_PREPROC_IDFR: {
					preproc_def_data def_data = w->data.def_data;
					token def_code = def_data.def_code;
					output_tokens = sabr_compiler_preprocess_eval_token(comp, def_code, def_data.is_func, output_tokens);
					if (!output_tokens) {
						fprintf(stderr, console_yellow console_bold "%s" console_reset " in line %zu, column %zu\n", t.data, t.begin_pos.line, t.begin_pos.column);
						fprintf(stderr, "in file " console_yellow console_bold "%s\n" console_reset, *vector_at(cctl_ptr(char), &comp->filename_vector, t.textcode_index));
						goto FREE_ALL;
					}
				} break;
				default:
					break;
			}
			if (preproc_stop) break;
		}
		else {
			new_t_data = NULL;
			token new_t = {0, };
			new_t = t;

			new_t_data = sabr_new_string_copy(t.data);
			if (!new_t_data) {
				fputs(sabr_errmsg_alloc, stderr);
				goto FREE_ALL;
			}

			ssize_t brace_stack = 0;
			bool brace_existence = false;
			for (char* ch = new_t_data; *ch != '\0'; ch++) {
				if (*ch == '{') {
					brace_stack++;
					brace_existence = true;
				}
				else if (*ch == '}') {
					brace_stack--;
					brace_existence = true;
				}
			}

			bool is_code_block = *new_t_data == '{';
			bool is_string = (*new_t_data == '\'') || *new_t_data == '\"';

			if (
				(!is_string && (brace_stack != 0)) ||
				(!is_string && brace_existence && !is_code_block)
			) {
				fputs(sabr_errmsg_wrong_token_fmt, stderr);
				fprintf(stderr, console_yellow console_bold "%s" console_reset " in line %zu, column %zu\n", t.data, t.begin_pos.line, t.begin_pos.column);
				fprintf(stderr, "in file " console_yellow console_bold "%s\n" console_reset, *vector_at(cctl_ptr(char), &comp->filename_vector, t.textcode_index));
				goto FREE_ALL;
			}

			new_t.data = new_t_data;
			new_t_data = NULL;

			if (!vector_push_back(token, output_tokens, new_t)) {
				fputs(sabr_errmsg_alloc, stderr);
				goto FREE_ALL;
			}
		}
	}

	return output_tokens;

FREE_ALL:
	sabr_free_token_vector(output_tokens);
	free(output_tokens);

	free(new_t_data);
	return NULL;
}

vector(token)* sabr_compiler_preprocess_eval_token(sabr_compiler* const comp, token t, bool has_local, vector(token)* output_tokens) {
	bool result = false;
	vector(token)* input_tokens = NULL;
	trie(word)* preproc_local_dictionary = NULL;
	char* temp_input_string = NULL;
	char* input_string = NULL;
	pos input_pos = t.begin_pos;
	size_t input_index = t.textcode_index;

	if (t.data[0] == '{') {
		temp_input_string = sabr_new_string_slice(t.data, 1, strlen(t.data) - 1);
		if (!t.is_generated) input_pos.column++;
	}
	else temp_input_string = sabr_new_string_copy(t.data);

	input_string = sabr_new_string_append(temp_input_string, " \n\0");

	input_tokens = sabr_compiler_tokenize_string(comp, input_string, input_index, input_pos, t.is_generated);
	if (!input_tokens) {
		fputs(sabr_errmsg_tokenize, stderr);
		goto FREE_ALL;
	}

	if (has_local) {
		preproc_local_dictionary = (trie(word)*) malloc(sizeof(trie(word)));
		if (!preproc_local_dictionary) {
			fputs(sabr_errmsg_alloc, stderr);
			goto FREE_ALL;
		}

		trie_init(word, preproc_local_dictionary);
		if (!vector_push_back(cctl_ptr(trie(word)), &comp->preproc_local_dictionary_stack, preproc_local_dictionary)) {
			fputs(sabr_errmsg_alloc, stderr);
			goto FREE_ALL;
		}
	}

	output_tokens = sabr_compiler_preprocess_tokens(comp, input_tokens, output_tokens);
	if (!output_tokens) {
		fputs(sabr_errmsg_preprocess, stderr);
		goto FREE_ALL;
	}
	free(input_string);
	free(temp_input_string);

	result = true;
FREE_ALL:
	if (!result) {
		free(input_string);
		free(temp_input_string);

		sabr_free_token_vector(input_tokens);
		free(input_tokens);

		sabr_free_token_vector(output_tokens);
		free(output_tokens);

		input_tokens = NULL;
		output_tokens = NULL;
	}

	if (has_local) {
		sabr_free_word_trie(preproc_local_dictionary);
		free(preproc_local_dictionary);
		vector_pop_back(cctl_ptr(trie(word)), &comp->preproc_local_dictionary_stack);
		vector_pop_back(preproc_stop_flag, &comp->preproc_stop_stack);
	}

	return output_tokens;;
}

bool sabr_compiler_preprocess_parse_value(sabr_compiler* const comp, token t, value* v) {
	switch (t.data[0]) {
		case '+':
			return sabr_compiler_parse_zero_begin_num(t.data, 1, false, v);
		case '-':
			return sabr_compiler_parse_zero_begin_num(t.data, 1, true, v);
		case '0':
			return sabr_compiler_parse_zero_begin_num(t.data, 0, false, v);
		case '.':
			return sabr_compiler_parse_num(t.data, v);
		case '1' ... '9':
			return sabr_compiler_parse_num(t.data, v);
		default:
			v->u = 0;
			return true;
	}
}

vector(token)* sabr_compiler_tokenize_string(sabr_compiler* const comp, const char* textcode, size_t textcode_index, pos init_pos, bool is_generated) {
	size_t current_index = 0;
	size_t begin_index = 0;
	size_t end_index = 0;

	pos current_pos = init_pos;
	pos begin_pos = init_pos;
	pos end_pos = {0, };

	size_t brace_level = 0;

	char* error_token_str = NULL;

	comment_parse_mode comment = CMNT_PARSE_NONE;
	string_parse_mode string_parse = STR_PARSE_NONE;
	bool string_escape = false;
	bool string_parsed = false;
	bool space = true;

	bool break_flag = false;
	
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
				if (character == '\t')
					current_pos.column += comp->tab_size - 1;
				
				if (!comment) {
					if (!space) {
						if (string_parse) {
							if (string_escape) string_escape = false;
						}
						else {
							token t = {0, };

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
							string_parsed = false;
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
							string_parsed = true;
						}
					}
					else if (space) {
						space = false;
						begin_index = current_index;
						begin_pos = current_pos;
						string_parse = STR_PARSE_SINGLE;
						string_escape = false;
					}
					else {
						goto WRONG_TOKEN;
					}
				}
			} break;
			case '\"': {
				if (!comment) {
					if (string_parse) {
						if (string_escape) string_escape = false;
						else if (string_parse == STR_PARSE_DOUBLE) {
							string_parse = STR_PARSE_NONE;
							string_parsed = true;
						}
					}
					else if (space) {
						space = false;
						begin_index = current_index;
						begin_pos = current_pos;
						string_parse = STR_PARSE_DOUBLE;
						string_escape = false;
					}
					else {
						goto WRONG_TOKEN;
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
					else {
						goto WRONG_TOKEN;
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
								string_parsed = true;
							}
						}
					}
					else if (space) {
						space = false;
						begin_index = current_index;
						begin_pos = current_pos;
					}
					else {
						goto WRONG_TOKEN;
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
					else if (string_parsed) {
						goto WRONG_TOKEN;
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

	if (string_parse) {
		current_index = begin_index;
		character = *(textcode + current_index);
		goto WRONG_TOKEN;
	}

	return tokens;

FREE_ALL:
	sabr_free_token_vector(tokens);
	free(tokens);

	return NULL;

WRONG_TOKEN:
	fputs(sabr_errmsg_wrong_token_fmt, stderr);

	break_flag = false;
	while (character) {
		switch (character) {
			case '\n': case '\r': case '\t': case ' ':
				break_flag = true;
			break;
		}
		if (break_flag) break;
		current_index++;
		character = *(textcode + current_index);
	}
	error_token_str = sabr_new_string_slice(textcode, begin_index, current_index);
	if (!error_token_str) {
		fputs(sabr_errmsg_alloc, stderr);
		goto FREE_ALL;
	}

	fprintf(stderr, console_yellow console_bold "%s" console_reset " in line %zu, column %zu\n", error_token_str, begin_pos.line, begin_pos.column);
	fprintf(stderr, "in file " console_yellow console_bold "%s\n" console_reset, *vector_at(cctl_ptr(char), &comp->filename_vector, textcode_index));
	goto FREE_ALL;
}

bytecode* sabr_compiler_compile_tokens(sabr_compiler* const comp, vector(token)* tokens) {
	bool result = false;

	bytecode* bc_data = (bytecode*) malloc(sizeof(bytecode));

	if (!bc_data) {
		fputs(sabr_errmsg_alloc, stderr);
		return NULL;
	}
	sabr_bytecode_init(bc_data);

	value value_a;
	value value_b;
	vector(value)* string_values = NULL;

	token current_token = {0, };

	for (size_t i = 0; i < tokens->size; i++) {
		current_token = *vector_at(token, tokens, i);
		word* w = NULL;
		w = trie_find(word, &comp->dictionary, current_token.data);
		if (w) {
			switch (w->type) {
				case WT_KWRD:
					if (!sabr_compiler_compile_keyword(comp, bc_data, w->data.kwrd)) goto PRINT_ERR_POS;
					break;
				case WT_IDFR:
					value_a.u = w->data.identifer_index;
					if (!sabr_bytecode_write_bcop_with_value(bc_data, OP_EXEC, value_a)) goto PRINT_ERR_POS;
					break;
				case WT_OP:
					if (!sabr_bytecode_write_bcop(bc_data, w->data.oc)) goto PRINT_ERR_POS;
					break;
				default:
					goto PRINT_ERR_POS;
			}
		}
		else {
			switch (current_token.data[0]) {
				case '+':
					if (!sabr_compiler_parse_zero_begin_num(current_token.data, 1, false, &value_a)) goto PRINT_ERR_POS;
					if (!sabr_bytecode_write_bcop_with_value(bc_data, OP_VALUE, value_a)) goto PRINT_ERR_POS;
					break;
				case '-':
					if (!sabr_compiler_parse_zero_begin_num(current_token.data, 1, true, &value_a)) goto PRINT_ERR_POS;
					if (!sabr_bytecode_write_bcop_with_value(bc_data, OP_VALUE, value_a)) goto PRINT_ERR_POS;
					break;
				case '0':
					if (!sabr_compiler_parse_zero_begin_num(current_token.data, 0, false, &value_a)) goto PRINT_ERR_POS;
					if (!sabr_bytecode_write_bcop_with_value(bc_data, OP_VALUE, value_a)) goto PRINT_ERR_POS;
					break;
				case '.':
				case '1' ... '9':
					if (!sabr_compiler_parse_num(current_token.data, &value_a)) goto PRINT_ERR_POS;
					if (!sabr_bytecode_write_bcop_with_value(bc_data, OP_VALUE, value_a)) goto PRINT_ERR_POS;
					break;
				case '$':
					if (!sabr_compiler_parse_identifier(comp, current_token.data + 1, &value_a)) goto PRINT_ERR_POS;
					if (!sabr_bytecode_write_bcop_with_value(bc_data, OP_VALUE, value_a)) goto PRINT_ERR_POS;
					break;
				case '{':
					fputs(sabr_errmsg_unused_preproc_token, stderr);
					goto PRINT_ERR_POS;
					break;
				case '\'':
					string_values = sabr_compiler_parse_string(comp, current_token.data);
					if (!string_values) goto PRINT_ERR_POS;
					for (size_t j = string_values->size - 1; j < -1; j--) {
						value_a = *vector_at(value, string_values, j);
						if (!sabr_bytecode_write_bcop_with_value(bc_data, OP_VALUE, value_a)) goto PRINT_ERR_POS;
					}
					vector_free(value, string_values);
					free(string_values);
					string_values = NULL;
					break;
				case '\"':
					string_values = sabr_compiler_parse_string(comp, current_token.data);
					if (!string_values) goto PRINT_ERR_POS;
					
					if (!sabr_bytecode_write_bcop(bc_data, OP_ARRAY)) goto PRINT_ERR_POS;

					for (size_t j = 0; j < string_values->size; j++) {
						value_a = *vector_at(value, string_values, j);
						if (!sabr_bytecode_write_bcop_with_value(bc_data, OP_VALUE, value_a)) goto PRINT_ERR_POS;
						if (j != string_values->size - 1) {
							if (!sabr_bytecode_write_bcop(bc_data, OP_ARRAY_COMMA)) goto PRINT_ERR_POS;
						}
					}
					
					if (!sabr_bytecode_write_bcop(bc_data, OP_ARRAY_END)) goto PRINT_ERR_POS;

					vector_free(value, string_values);
					free(string_values);
					string_values = NULL;
					break;
				default:
					if (!sabr_compiler_parse_struct_member(comp, current_token.data, &value_a, &value_b)) goto PRINT_ERR_POS;
					if (!sabr_bytecode_write_bcop_with_value(bc_data, OP_VALUE, value_a)) goto PRINT_ERR_POS;
					if (!sabr_bytecode_write_bcop_with_value(bc_data, OP_VALUE, value_b)) goto PRINT_ERR_POS;
					if (!sabr_bytecode_write_bcop(bc_data, OP_STRUCT_EXEC)) goto PRINT_ERR_POS;
			}
		}
	}

	result = true;

FREE_ALL:
	if (string_values) {
		vector_free(value, string_values);
		free(string_values);
	}
	if (!result) {
		vector_free(bytecode_operation, &bc_data->bcop_vec);
		free(bc_data);
		bc_data = NULL;
	}
	return bc_data;

PRINT_ERR_POS:
	fprintf(stderr, console_yellow console_bold "%s" console_reset " in line %zu, column %zu\n", current_token.data, current_token.begin_pos.line, current_token.begin_pos.column);
	fprintf(stderr, "in file " console_yellow console_bold "%s\n" console_reset, *vector_at(cctl_ptr(char), &comp->filename_vector, current_token.textcode_index));
	goto FREE_ALL;
}

bool sabr_compiler_parse_zero_begin_num(const char* str, size_t index, bool negate, value* v) {
	size_t len = strlen(str);

	if (len > 2) {
		if (str[index] == '0') {
			switch (str[index + 1]) {
				case 'x':
					return sabr_compiler_parse_base_n_num(str, index, negate, 16, v);
				case 'o':
					return sabr_compiler_parse_base_n_num(str, index, negate, 8, v);
				case 'b':
					return sabr_compiler_parse_base_n_num(str, index, negate, 2, v);
				default:
					return sabr_compiler_parse_num(str, v);
			}
		}
	}
	return sabr_compiler_parse_num(str, v);
}

bool sabr_compiler_parse_base_n_num(const char* str, size_t index, bool negate, int base, value* v) {
	const char* temp_str = str + index + 2;
	char* stop;
	errno = 0;
	v->i = strtoll(temp_str, &stop, base);
	if (errno == ERANGE) {
		if (v->i == LLONG_MAX) {
			v->u = strtoull(temp_str, &stop, base);
		}
	}
	if (negate) v->i = -v->i;
	if (*stop) {
		fputs(sabr_errmsg_parse_num, stderr);
		return false;
	}

	return true;
}

bool sabr_compiler_parse_num(const char* str, value* v) {
	char* stop;

	errno = 0;
	if (strchr(str, '.')) {
		v->f = strtod(str, &stop);
	}
	else {
		v->i = strtoll(str, &stop, 10);
		if (errno == ERANGE) {
			if (v->i == LLONG_MAX) {
				v->u = strtoull(str, &stop, 10);
			}
		}
	}
	if (*stop) {
		fputs(sabr_errmsg_parse_num, stderr);
		return false;
	}
	
	return true;
}

bool sabr_compiler_parse_identifier(sabr_compiler* const comp, const char* str, value* v) {
	word* w = NULL;
	w = trie_find(word, &comp->dictionary, str);

	if (w) {
		if (w->type != WT_IDFR) {
			fputs(sabr_errmsg_kwrd_ident, stderr);
			return false;
		}
		v->u = w->data.identifer_index;
	}
	else {
		if (!sabr_compiler_is_string_can_be_identifier(str)) {
			fputs(sabr_errmsg_invalid_ident_fmt, stderr); return false;
		}

		comp->identifier_count++;

		word new_identifier_word = {0, };
		
		new_identifier_word.type = WT_IDFR;
		new_identifier_word.data.identifer_index = comp->identifier_count;

		if (!trie_insert(word, &comp->dictionary, str, new_identifier_word)) {
			fputs(sabr_errmsg_alloc, stderr); return false;
		}
		v->u = comp->identifier_count;
	}
	return true;
}

bool sabr_compiler_is_string_can_be_identifier(const char* str) {
	const char* ch = str;
	size_t len = strlen(str);
	if (len < 0) return false;
	switch (*ch) {
		case '@': case '#': case '$':
		case '(': case ')': case '{': case '}':
		case '\\': case '\'': case '\"': case '\0':
			return false;
	}
	return true;
}

vector(value)* sabr_compiler_parse_string(sabr_compiler* const comp, const char* str) {
	char* ch = NULL;
	vector(value)* vec = (vector(value)*) malloc(sizeof(vector(value)));
	if (!vec) {
		fputs(sabr_errmsg_alloc, stderr);
		return NULL;
	}
	vector_init(value, vec);

	size_t len = strlen(str);
	ch = sabr_new_string_slice(str, 1, len - 1);
	if (!ch) {
		fputs(sabr_errmsg_alloc, stderr);
		goto FAILURE;
	}
	char* end = ch + len - 1;

	size_t num_parse_count;

	value v;

	while (*ch) {
		if (((signed char) *ch) > -1) {
			char* num_parse_stop = NULL;
			char num_parse[9] = {0, };
			num_parse_stop = 0;

			if (*ch == '\\') {
				ch++;
				switch (*ch) {
					case 'a': v.u = '\a'; break; case 'b': v.u = '\b'; break;
					case 'e': v.u = '\e'; break; case 'f': v.u = '\f'; break;
					case 'n': v.u = '\n'; break; case 'r': v.u = '\r'; break;
					case 't': v.u = '\t'; break; case 'v': v.u = '\v'; break;
					case '\\': v.u = '\\'; break; case '\'': v.u = '\''; break; case '\"': v.u = '\"'; break;
					case '0' ... '7':
						while ((*ch >= '0') && (*ch <= '7') && (num_parse_count < 3)) {
							num_parse[num_parse_count] = *ch;
							ch++;
							num_parse_count++;
						}
						ch--;
						v.u = strtoull(num_parse, &num_parse_stop, 8);
						break;
					case 'x':
						if (!sabr_compiler_parse_string_escape_hex(
							&ch, num_parse_stop, num_parse, &num_parse_count, &v, 2
						)) {
							fputs(sabr_errmsg_wrong_escape_sequence, stderr); goto FAILURE;
						}
						break;
					case 'u':
						if (!sabr_compiler_parse_string_escape_hex(
							&ch, num_parse_stop, num_parse, &num_parse_count, &v, 4
						)) {
							fputs(sabr_errmsg_wrong_escape_sequence, stderr); goto FAILURE;
						}
						break;
					case 'U':
						if (!sabr_compiler_parse_string_escape_hex(
							&ch, num_parse_stop, num_parse, &num_parse_count, &v, 8
						)) {
							fputs(sabr_errmsg_wrong_escape_sequence, stderr); goto FAILURE;
						}
						break;
					default: fputs(sabr_errmsg_wrong_escape_sequence, stderr); goto FAILURE;
				}
			}
			else if ((*ch == '\'') || (*ch == '\"')) {
				goto FAILURE;
			}
			else {
				v.u = *ch;
			}
			ch++;
		}
		else {
			char32_t out;
			size_t rc;
			rc = mbrtoc32(&out, ch, end - ch, &(comp->convert_state));
			if ((rc > ((size_t) -4)) || (rc == 0)) {
				goto FAILURE;
			}
			ch += rc;
			v.u = out;
		}
		if (!vector_push_back(value, vec, v)) {
			fputs(sabr_errmsg_alloc, stderr);
			goto FAILURE;
		}
	}

	return vec;
FAILURE:
	fputs(sabr_errmsg_parse_str, stderr);
	vector_free(value, vec);
	free(vec);
	free(ch);
	return NULL;
}

bool sabr_compiler_parse_string_escape_hex(char** ch_addr, char* num_parse_stop, char* num_parse, size_t* num_parse_count, value* v, int length) {
	(*ch_addr)++;

	while (
		(
			((**ch_addr >= '0') && (**ch_addr <= '9')) ||
			((**ch_addr >= 'a') && (**ch_addr <= 'f')) ||
			((**ch_addr >= 'A') && (**ch_addr <= 'F'))
		) && ((*num_parse_count) < length)
	) {
		num_parse[*num_parse_count] = **ch_addr;
		(*ch_addr)++;
		(*num_parse_count)++;
	}
	if ((*num_parse_count) != length) {
		return false;
	}
	(*ch_addr)--;
	v->u = strtoull(num_parse, &num_parse_stop, 16);
	return true;
}

bool sabr_compiler_parse_struct_member(sabr_compiler* const comp, const char* str, value* struct_v, value* member_v) {
	bool result = false;
	char* struct_str = NULL;
	char* member_str = NULL;
	char* find = strchr(str, '.');

	if (!find) goto WRONG;

	size_t len = strlen(str);
	size_t index = (find - str);

	struct_str = sabr_new_string_slice(str, 0, index);
	member_str = sabr_new_string_slice(str, index + 1, len);
	if (!(struct_str && member_str)) {
		fputs(sabr_errmsg_alloc, stderr);
		goto FREE_ALL;
	}

	word* w = NULL;
	w = trie_find(word, &comp->dictionary, struct_str);
	if (!w) goto WRONG;
	if (w->type != WT_IDFR) goto WRONG;
	if (!sabr_compiler_parse_identifier(comp, struct_str, struct_v)) goto WRONG;

	w = NULL;
	w = trie_find(word, &comp->dictionary, member_str);
	if (!w) goto WRONG;
	if (w->type != WT_IDFR) goto WRONG;
	if (!sabr_compiler_parse_identifier(comp, member_str, member_v)) goto WRONG;

	result = true;

FREE_ALL:
	if (struct_str) free(struct_str);
	if (member_str) free(member_str);
	return result;

WRONG:
	fputs(sabr_errmsg_wrong_ident, stderr);
	goto FREE_ALL;
}

bool sabr_compiler_compile_keyword(sabr_compiler* const comp, bytecode* bc_data, keyword kwrd) {
	keyword_data current_kd;
	current_kd.kwrd = kwrd;
	current_kd.index = bc_data->current_index;
	current_kd.pos = bc_data->current_pos;

	vector(keyword_data)* temp_kd_vec = NULL;

	switch (current_kd.kwrd) {
		case KWRD_IF:
			temp_kd_vec = (vector(keyword_data)*) malloc(sizeof(vector(keyword_data)));
			if (!temp_kd_vec) goto FAILURE_ALLOC;
			vector_init(keyword_data, temp_kd_vec);
			if (!vector_push_back(keyword_data, temp_kd_vec, current_kd)) goto FAILURE_ALLOC;
			if (!vector_push_back(cctl_ptr(vector(keyword_data)), &comp->keyword_data_stack, temp_kd_vec))
				goto FAILURE_ALLOC;
			if (!sabr_bytecode_write_bcop_with_null(bc_data, OP_IF)) goto FREE_ALL;
			break;
		case KWRD_ELSE:
			if (!comp->keyword_data_stack.size) goto FAILURE_WRONG;
			temp_kd_vec = *vector_back(cctl_ptr(vector(keyword_data)), &comp->keyword_data_stack);
			if (!vector_push_back(keyword_data, temp_kd_vec, current_kd)) goto FAILURE_ALLOC;
			if (!sabr_bytecode_write_bcop_with_null(bc_data, OP_JUMP)) goto FREE_ALL;
			break;
		case KWRD_LOOP:
			temp_kd_vec = (vector(keyword_data)*) malloc(sizeof(vector(keyword_data)));
			if (!temp_kd_vec) goto FAILURE_ALLOC;
			vector_init(keyword_data, temp_kd_vec);
			if (!vector_push_back(keyword_data, temp_kd_vec, current_kd)) goto FAILURE_ALLOC;
			if (!vector_push_back(cctl_ptr(vector(keyword_data)), &comp->keyword_data_stack, temp_kd_vec))
				goto FAILURE_ALLOC;
			break;
		case KWRD_WHILE:
			if (!comp->keyword_data_stack.size) goto FAILURE_WRONG;
			temp_kd_vec = *vector_back(cctl_ptr(vector(keyword_data)), &comp->keyword_data_stack);
			if (!vector_push_back(keyword_data, temp_kd_vec, current_kd)) goto FAILURE_ALLOC;
			if (!sabr_bytecode_write_bcop_with_null(bc_data, OP_IF)) goto FREE_ALL;
			break;
		case KWRD_BREAK:
		case KWRD_CONTINUE:
			if (!comp->keyword_data_stack.size) goto FAILURE_WRONG;
			temp_kd_vec = *vector_back(cctl_ptr(vector(keyword_data)), &comp->keyword_data_stack);
			if (!vector_push_back(keyword_data, temp_kd_vec, current_kd)) goto FAILURE_ALLOC;
			if (!sabr_bytecode_write_bcop_with_null(bc_data, OP_JUMP)) goto FREE_ALL;
			break;
		case KWRD_FOR:
		case KWRD_UFOR:
		case KWRD_FFOR: {
			value for_type;
			temp_kd_vec = (vector(keyword_data)*) malloc(sizeof(vector(keyword_data)));
			if (!temp_kd_vec) goto FAILURE_ALLOC;
			vector_init(keyword_data, temp_kd_vec);
			if (!vector_push_back(keyword_data, temp_kd_vec, current_kd)) goto FAILURE_ALLOC;
			if (!vector_push_back(cctl_ptr(vector(keyword_data)), &comp->keyword_data_stack, temp_kd_vec))
				goto FAILURE_ALLOC;
			
			switch (current_kd.kwrd) {
				case KWRD_FOR: for_type.u = 0; break;
				case KWRD_UFOR: for_type.u = 1; break;
				case KWRD_FFOR: for_type.u = 2; break;
				default: break;
			}
			for_type.u = current_kd.kwrd - KWRD_FOR;

			if (!sabr_bytecode_write_bcop_with_value(bc_data, OP_FOR, for_type)) goto FREE_ALL;
			if (!sabr_bytecode_write_bcop(bc_data, OP_NONE)) goto FREE_ALL;
		} break;
		case KWRD_FROM:
			if (!comp->keyword_data_stack.size) goto FAILURE_WRONG;
			temp_kd_vec = *vector_back(cctl_ptr(vector(keyword_data)), &comp->keyword_data_stack);
			if (!vector_push_back(keyword_data, temp_kd_vec, current_kd)) goto FAILURE_ALLOC;
			if (!sabr_bytecode_write_bcop(bc_data, OP_FOR_FROM)) goto FREE_ALL;
			if (!sabr_bytecode_write_bcop(bc_data, OP_NONE)) goto FREE_ALL;
			break;
		case KWRD_TO:
			if (!comp->keyword_data_stack.size) goto FAILURE_WRONG;
			temp_kd_vec = *vector_back(cctl_ptr(vector(keyword_data)), &comp->keyword_data_stack);
			if (!vector_push_back(keyword_data, temp_kd_vec, current_kd)) goto FAILURE_ALLOC;
			if (!sabr_bytecode_write_bcop(bc_data, OP_FOR_TO)) goto FREE_ALL;
			if (!sabr_bytecode_write_bcop(bc_data, OP_NONE)) goto FREE_ALL;
			break;
		case KWRD_STEP:
			if (!comp->keyword_data_stack.size) goto FAILURE_WRONG;
			temp_kd_vec = *vector_back(cctl_ptr(vector(keyword_data)), &comp->keyword_data_stack);
			if (!vector_push_back(keyword_data, temp_kd_vec, current_kd)) goto FAILURE_ALLOC;
			if (!sabr_bytecode_write_bcop(bc_data, OP_FOR_STEP)) goto FREE_ALL;
			if (!sabr_bytecode_write_bcop(bc_data, OP_NONE)) goto FREE_ALL;
			break;
		case KWRD_SWITCH:
			temp_kd_vec = (vector(keyword_data)*) malloc(sizeof(vector(keyword_data)));
			if (!temp_kd_vec) goto FAILURE_ALLOC;
			vector_init(keyword_data, temp_kd_vec);
			if (!vector_push_back(keyword_data, temp_kd_vec, current_kd)) goto FAILURE_ALLOC;
			if (!vector_push_back(cctl_ptr(vector(keyword_data)), &comp->keyword_data_stack, temp_kd_vec))
				goto FAILURE_ALLOC;
			if (!sabr_bytecode_write_bcop(bc_data, OP_SWITCH)) goto FREE_ALL;
			break;
		case KWRD_CASE:
			if (!comp->keyword_data_stack.size) goto FAILURE_WRONG;
			temp_kd_vec = *vector_back(cctl_ptr(vector(keyword_data)), &comp->keyword_data_stack);
			if (!vector_push_back(keyword_data, temp_kd_vec, current_kd)) goto FAILURE_ALLOC;
			if (!sabr_bytecode_write_bcop(bc_data, OP_SWITCH_CASE)) goto FREE_ALL;
			if (!sabr_bytecode_write_bcop(bc_data, OP_EQU)) goto FREE_ALL;
			if (!sabr_bytecode_write_bcop_with_null(bc_data, OP_IF)) goto FREE_ALL;
			break;
		case KWRD_PASS:
			if (!comp->keyword_data_stack.size) goto FAILURE_WRONG;
			temp_kd_vec = *vector_back(cctl_ptr(vector(keyword_data)), &comp->keyword_data_stack);
			if (!vector_push_back(keyword_data, temp_kd_vec, current_kd)) goto FAILURE_ALLOC;
			if (!sabr_bytecode_write_bcop_with_null(bc_data, OP_JUMP)) goto FREE_ALL;
			break;
		case KWRD_FUNC:
			temp_kd_vec = (vector(keyword_data)*) malloc(sizeof(vector(keyword_data)));
			if (!temp_kd_vec) goto FAILURE_ALLOC;
			vector_init(keyword_data, temp_kd_vec);
			if (!vector_push_back(keyword_data, temp_kd_vec, current_kd)) goto FAILURE_ALLOC;
			if (!vector_push_back(cctl_ptr(vector(keyword_data)), &comp->keyword_data_stack, temp_kd_vec))
				goto FAILURE_ALLOC;
			if (!sabr_bytecode_write_bcop_with_null(bc_data, OP_LAMBDA)) goto FREE_ALL;
			if (!sabr_bytecode_write_bcop(bc_data, OP_LOCAL)) goto FREE_ALL;
			break;
		case KWRD_MACRO:
			temp_kd_vec = (vector(keyword_data)*) malloc(sizeof(vector(keyword_data)));
			if (!temp_kd_vec) goto FAILURE_ALLOC;
			vector_init(keyword_data, temp_kd_vec);
			if (!vector_push_back(keyword_data, temp_kd_vec, current_kd)) goto FAILURE_ALLOC;
			if (!vector_push_back(cctl_ptr(vector(keyword_data)), &comp->keyword_data_stack, temp_kd_vec))
				goto FAILURE_ALLOC;
			if (!sabr_bytecode_write_bcop_with_null(bc_data, OP_LAMBDA)) goto FREE_ALL;
			break;
		case KWRD_DEFER:
			if (!comp->keyword_data_stack.size) goto FAILURE_WRONG;
			temp_kd_vec = *vector_back(cctl_ptr(vector(keyword_data)), &comp->keyword_data_stack);
			if (!vector_push_back(keyword_data, temp_kd_vec, current_kd)) goto FAILURE_ALLOC;
			break;
		case KWRD_RETURN:
			if (!comp->keyword_data_stack.size) goto FAILURE_WRONG;
			temp_kd_vec = *vector_back(cctl_ptr(vector(keyword_data)), &comp->keyword_data_stack);
			if (!vector_push_back(keyword_data, temp_kd_vec, current_kd)) goto FAILURE_ALLOC;
			if (!sabr_bytecode_write_bcop_with_null(bc_data, OP_JUMP)) goto FREE_ALL;
			break;
		case KWRD_STRUCT:
			temp_kd_vec = (vector(keyword_data)*) malloc(sizeof(vector(keyword_data)));
			if (!temp_kd_vec) goto FAILURE_ALLOC;
			vector_init(keyword_data, temp_kd_vec);
			if (!vector_push_back(keyword_data, temp_kd_vec, current_kd)) goto FAILURE_ALLOC;
			if (!vector_push_back(cctl_ptr(vector(keyword_data)), &comp->keyword_data_stack, temp_kd_vec))
				goto FAILURE_ALLOC;
			if (!sabr_bytecode_write_bcop(bc_data, OP_STRUCT)) goto FREE_ALL;
			break;
		case KWRD_MEMBER:
			if (!comp->keyword_data_stack.size) goto FAILURE_WRONG;
			temp_kd_vec = *vector_back(cctl_ptr(vector(keyword_data)), &comp->keyword_data_stack);
			if (!vector_push_back(keyword_data, temp_kd_vec, current_kd)) goto FAILURE_ALLOC;
			if (!sabr_bytecode_write_bcop(bc_data, OP_MEMBER)) goto FREE_ALL;
			break;
		case KWRD_END: {
			value pos;
			if (comp->keyword_data_stack.size == 0) goto FAILURE_WRONG;
			temp_kd_vec = *vector_back(cctl_ptr(vector(keyword_data)), &comp->keyword_data_stack);
			keyword_data* first_kd = vector_front(keyword_data, temp_kd_vec);
			keyword_data* inner_kd = NULL;

			switch (first_kd->kwrd) {
				case KWRD_IF: {
					bytecode_operation* if_bcop = sabr_compiler_get_bcop(bc_data, first_kd->index);
					if (!if_bcop) goto FREE_ALL;
					for (size_t i = 1; i < temp_kd_vec->size; i++) {
						keyword_data* iter_kd = vector_at(keyword_data, temp_kd_vec, i);
						switch (iter_kd->kwrd) {
							case KWRD_ELSE:
								if (inner_kd) goto FAILURE_WRONG;
								inner_kd = iter_kd;
								break;
							case KWRD_COMMA: case KWRD_BREAK: case KWRD_CONTINUE: case KWRD_RETURN: {
								vector(keyword_data)* next_kd_vec;
								if (comp->keyword_data_stack.size < 2) goto FAILURE_WRONG;
								next_kd_vec = *vector_at(cctl_ptr(vector(keyword_data)), &comp->keyword_data_stack, comp->keyword_data_stack.size - 2);
								if (!vector_push_back(keyword_data, next_kd_vec, *iter_kd)) goto FAILURE_ALLOC;
							} break;
							default: goto FAILURE_WRONG;
						}
					}
					if (inner_kd) {
						bytecode_operation* else_bcop = sabr_compiler_get_bcop(bc_data, inner_kd->index);
						if (!else_bcop) goto FREE_ALL;

						// pos.u = inner_kd->pos + 9;
						pos.u = inner_kd->index + 1;
						if_bcop->operand = pos;
						// pos.u = current_kd.pos;
						pos.u = current_kd.index;
						else_bcop->operand = pos;
					}
					else {
						// pos.u = current_kd.pos;
						pos.u = current_kd.index;
						if_bcop->operand = pos;
					}
				} break;
				case KWRD_LOOP: {
					bytecode_operation* temp_bcop = NULL;
					for (size_t i = 1; i < temp_kd_vec->size; i++) {
						keyword_data* iter_kd = vector_at(keyword_data, temp_kd_vec, i);
						switch (iter_kd->kwrd) {
							case KWRD_WHILE:
								if (inner_kd) goto FAILURE_WRONG;
								inner_kd = iter_kd;
								// pos.u = current_kd.pos + 9;
								pos.u = current_kd.index + 1;
								temp_bcop = sabr_compiler_get_bcop(bc_data, inner_kd->index);
								if (!temp_bcop) goto FREE_ALL;
								temp_bcop->operand = pos;
								break;
							case KWRD_BREAK:
								// pos.u = current_kd.pos + 9;
								pos.u = current_kd.index + 1;
								temp_bcop = sabr_compiler_get_bcop(bc_data, iter_kd->index);
								if (!temp_bcop) goto FREE_ALL;
								temp_bcop->operand = pos;
								break;
							case KWRD_CONTINUE:
								// pos.u = first_kd->pos;
								pos.u = first_kd->index;
								temp_bcop = sabr_compiler_get_bcop(bc_data, iter_kd->index);
								if (!temp_bcop) goto FREE_ALL;
								temp_bcop->operand = pos;
								break;
							case KWRD_COMMA: case KWRD_RETURN: {
								vector(keyword_data)* next_kd_vec;
								if (comp->keyword_data_stack.size < 2) goto FAILURE_WRONG;
								next_kd_vec = *vector_at(cctl_ptr(vector(keyword_data)), &comp->keyword_data_stack, comp->keyword_data_stack.size - 2);
								if (!vector_push_back(keyword_data, next_kd_vec, *iter_kd)) goto FAILURE_ALLOC;
							} break;
							default: goto FAILURE_WRONG;
						}
					}
					// pos.u = first_kd->pos;
					pos.u = first_kd->index;
					if (!sabr_bytecode_write_bcop_with_value(bc_data, OP_JUMP, pos)) goto FREE_ALL;
				} break;
				case KWRD_FOR: case KWRD_UFOR: case KWRD_FFOR: {
					#define free_for_vecs() \
						vector_free(keyword_data, &continue_vec);
					bool exists_from = false;
					bool exists_to = false;
					bool exists_step = false;
					keyword_data* check_kd = first_kd;
					value check_pos = {0, };
					size_t check_index = 0;

					bytecode_operation* temp_bcop = NULL;

					vector(keyword_data) continue_vec;
					vector_init(keyword_data, &continue_vec);

					for (size_t i = 1; i < temp_kd_vec->size; i++) {
						keyword_data* iter_kd = vector_at(keyword_data, temp_kd_vec, i);
						switch (iter_kd->kwrd) {
							case KWRD_FROM:
								if (exists_from) { free_for_vecs(); goto FAILURE_WRONG; }
								exists_from = true;
								check_kd = iter_kd;
								break;
							case KWRD_TO:
								if (exists_to) { free_for_vecs(); goto FAILURE_WRONG; }
								exists_to = true;
								check_kd = iter_kd;
								break;
							case KWRD_STEP:
								if (exists_step) { free_for_vecs(); goto FAILURE_WRONG; }
								exists_step = true;
								check_kd = iter_kd;
								break;
							case KWRD_BREAK:
								// pos.u = current_kd.pos + 9;
								pos.u = current_kd.index + 1;
								temp_bcop = sabr_compiler_get_bcop(bc_data, iter_kd->index);
								if (!temp_bcop) goto FREE_ALL;
								temp_bcop->operand = pos;
								break;
							case KWRD_CONTINUE:
								if (!vector_push_back(keyword_data, &continue_vec, *iter_kd)) {
									free_for_vecs(); goto FAILURE_ALLOC;
								}
								break;
							case KWRD_COMMA: case KWRD_RETURN: {
								vector(keyword_data)* next_kd_vec;
								if (comp->keyword_data_stack.size < 2) { free_for_vecs(); goto FAILURE_WRONG; }
								next_kd_vec = *vector_at(cctl_ptr(vector(keyword_data)), &comp->keyword_data_stack, comp->keyword_data_stack.size - 2);
								if (!vector_push_back(keyword_data, next_kd_vec, *iter_kd)) {
									free_for_vecs(); goto FAILURE_ALLOC;
								}
							} break;
							default: free_for_vecs(); goto FAILURE_WRONG;
						}
					}
					if (!(exists_from || exists_to || exists_step)) {
						// check_pos.u = check_kd->pos + 9;
						check_pos.u = check_kd->index + 1;
					}
					else {
						// check_pos.u = check_kd->pos + 1;
						check_pos.u = check_kd->index + 1;
					}
					check_index = check_kd->index + 1;
					// pos.u = current_kd.pos + 9;
					pos.u = current_kd.index + 1;

					bytecode_operation* check_bcop = sabr_compiler_get_bcop(bc_data, check_index);
					if (!check_bcop) goto FREE_ALL;
					check_bcop->oc = OP_FOR_CHECK;
					check_bcop->operand = pos;

					if (continue_vec.size > 0) {
						for (size_t i = 1; i < continue_vec.size; i++) {
							keyword_data* iter_kd = vector_at(keyword_data, &continue_vec, i);
							temp_bcop = sabr_compiler_get_bcop(bc_data, iter_kd->index);
							if (!temp_bcop) goto FREE_ALL;
							temp_bcop->oc = OP_FOR_NEXT;
							temp_bcop->operand = check_pos;
						}
					}

					if (!sabr_bytecode_write_bcop_with_value(bc_data, OP_FOR_NEXT, check_pos)) goto FREE_ALL;
					if (!sabr_bytecode_write_bcop(bc_data, OP_FOR_END)) goto FREE_ALL;

					free_for_vecs();
					#undef free_for_vecs
				} break;
				case KWRD_SWITCH: {
					#define free_switch_vecs() \
						vector_free(keyword_data, &case_vec); \
						vector_free(keyword_data, &pass_vec); \
						vector_free(keyword_data, &chain_vec);
					
					vector(keyword_data) case_vec;
					vector(keyword_data) pass_vec;
					vector(keyword_data) chain_vec;

					vector_init(keyword_data, &case_vec);
					vector_init(keyword_data, &pass_vec);
					vector_init(keyword_data, &chain_vec);

					// pos.u = current_kd.pos;
					pos.u = current_kd.index;
					
					bool chain = false;
					bool exists_case = false;
					bool exists_pass = false;

					bytecode_operation* temp_bcop;

					for (size_t i = 1; i < temp_kd_vec->size; i++) {
						keyword_data* iter_kd = vector_at(keyword_data, temp_kd_vec, i);
						switch (iter_kd->kwrd) {
							case KWRD_CASE:
								if (chain) {
									if (!vector_push_back(keyword_data, &pass_vec, *iter_kd)) {
										free_switch_vecs(); goto FAILURE_ALLOC;
									}
								}
								if (!vector_push_back(keyword_data, &case_vec, *iter_kd)) {
									free_switch_vecs(); goto FAILURE_ALLOC;
								}
								chain = true;
								exists_case = true;
								break;
							case KWRD_PASS:
								chain = false;
								temp_bcop = sabr_compiler_get_bcop(bc_data, iter_kd->index);
								if (!temp_bcop) goto FREE_ALL;
								temp_bcop->operand = pos;
								if (!vector_push_back(keyword_data, &pass_vec, *iter_kd)) {
									free_switch_vecs(); goto FAILURE_ALLOC;
								}
								exists_pass = true;
								break;
							case KWRD_COMMA: case KWRD_BREAK: case KWRD_CONTINUE: case KWRD_RETURN: {
								vector(keyword_data)* next_kd_vec;
								if (comp->keyword_data_stack.size < 2) { free_switch_vecs(); goto FAILURE_WRONG; }
								next_kd_vec = *vector_at(cctl_ptr(vector(keyword_data)), &comp->keyword_data_stack, comp->keyword_data_stack.size - 2);
								if (!vector_push_back(keyword_data, next_kd_vec, *iter_kd)) {
									free_switch_vecs(); goto FAILURE_ALLOC;
								}
							} break;
							default: free_switch_vecs(); goto FAILURE_WRONG;
						}
					}

					if (!(
						exists_pass && exists_case &&
						(exists_pass ? (vector_at(keyword_data, temp_kd_vec, 1)->kwrd == KWRD_CASE) : 0)
					)) {
						free_switch_vecs(); goto FAILURE_WRONG;
					}

					keyword_data* iter_case_kd = vector_front(keyword_data, &case_vec);
					keyword_data* iter_pass_kd = vector_front(keyword_data, &pass_vec);

					for (; iter_case_kd <= vector_back(keyword_data, &case_vec); iter_case_kd++) {
						if (iter_pass_kd->kwrd == KWRD_PASS) {
							if (chain_vec.size) {
								for (size_t i = 0; i < chain_vec.size; i++) {
									keyword_data* iter_chain_kd = vector_at(keyword_data, &chain_vec, i);
									// pos.u = iter_case_kd->pos + 11;
									pos.u = iter_case_kd->index + 3;

									temp_bcop = sabr_compiler_get_bcop(bc_data, iter_chain_kd->index + 2);
									if (!temp_bcop) goto FREE_ALL;
									temp_bcop->operand = pos;

									temp_bcop = sabr_compiler_get_bcop(bc_data, iter_chain_kd->index + 1);
									if (!temp_bcop) goto FREE_ALL;
									temp_bcop->oc = OP_NEQ;
								}
								vector_clear(keyword_data, &chain_vec);
							}

							// pos.u = iter_pass_kd->pos + 9;
							pos.u = iter_pass_kd->index + 1;

							temp_bcop = sabr_compiler_get_bcop(bc_data, iter_case_kd->index + 2);
							if (!temp_bcop) goto FREE_ALL;
							temp_bcop->operand = pos;
							iter_pass_kd++;
						}
						else {
							if (!vector_push_back(keyword_data, &chain_vec, *iter_case_kd)) {
								free_switch_vecs(); goto FAILURE_ALLOC;
							}
							iter_pass_kd++;
						}
					}

					if (!sabr_bytecode_write_bcop(bc_data, OP_SWITCH_END)) goto FREE_ALL;
					
					free_switch_vecs();
					#undef free_switch_vecs
				} break;
				case KWRD_FUNC: {
					#define free_func_vecs() \
						vector_free(keyword_data, &return_vec);
					
					bool exists_defer = false;
					keyword_data defer_kd;

					vector(keyword_data) return_vec;
					vector_init(keyword_data, &return_vec);

					for (size_t i = 1; i < temp_kd_vec->size; i++) {
						keyword_data* iter_kd = vector_at(keyword_data, temp_kd_vec, i);
						switch (iter_kd->kwrd) {
							case KWRD_DEFER:
								if (exists_defer) { free_func_vecs(); goto FAILURE_WRONG; }
								exists_defer = true;
								defer_kd = *iter_kd;
								break;
							case KWRD_RETURN:
								if (exists_defer) { free_func_vecs(); goto FAILURE_WRONG; }
								if (!vector_push_back(keyword_data, &return_vec, *iter_kd)) {
									free_func_vecs(); goto FAILURE_ALLOC;
								}
								break;
							default: free_func_vecs(); goto FAILURE_WRONG;
						}
					}

					if (return_vec.size > 0) {
						for (size_t i = 0; i < return_vec.size; i++) {
							keyword_data* iter_kd = vector_at(keyword_data, &return_vec, i);
							bytecode_operation* return_bcop = sabr_compiler_get_bcop(bc_data, iter_kd->index);
							if (!return_bcop) goto FREE_ALL;
							if (exists_defer)
								// pos.u = defer_kd.pos;
								pos.u = defer_kd.index;
							else
								// pos.u = current_kd.pos;
								pos.u = current_kd.index;
							return_bcop->operand = pos;
						}
					}

					// pos.u = current_kd.pos + 2;
					pos.u = current_kd.index + 2;
					bytecode_operation* end_bcop = sabr_compiler_get_bcop(bc_data, first_kd->index);
					if (!end_bcop) goto FREE_ALL;
					end_bcop->operand = pos;

					if (!sabr_bytecode_write_bcop(bc_data, OP_LOCAL_END)) goto FREE_ALL;
					if (!sabr_bytecode_write_bcop(bc_data, OP_RETURN)) goto FREE_ALL;
					if (!sabr_bytecode_write_bcop(bc_data, OP_DEFINE)) goto FREE_ALL;

					free_func_vecs();
				} break;
				case KWRD_MACRO: {
					bool exists_defer = false;
					keyword_data defer_kd = {0, };

					vector(keyword_data) return_vec;
					vector_init(keyword_data, &return_vec);

					for (size_t i = 1; i < temp_kd_vec->size; i++) {
						keyword_data* iter_kd = vector_at(keyword_data, temp_kd_vec, i);
						switch (iter_kd->kwrd) {
							case KWRD_DEFER:
								if (exists_defer) { free_func_vecs(); goto FAILURE_WRONG; }
								exists_defer = true;
								defer_kd = *iter_kd;
								break;
							case KWRD_RETURN:
								if (exists_defer) { free_func_vecs(); goto FAILURE_WRONG; }
								if (!vector_push_back(keyword_data, &return_vec, *iter_kd)) {
									free_func_vecs(); goto FAILURE_ALLOC;
								}
								break;
							default: free_func_vecs(); goto FREE_ALL;
						}
					}

					if (return_vec.size > 0) {
						for (size_t i = 0; i < return_vec.size; i++) {
							keyword_data* iter_kd = vector_at(keyword_data, &return_vec, i);
							bytecode_operation* return_bcop = sabr_compiler_get_bcop(bc_data, iter_kd->index);
							if (!return_bcop) goto FREE_ALL;
							if (exists_defer)
								// pos.u = defer_kd.pos;
								pos.u = defer_kd.index;
							else
								// pos.u = current_kd.pos;
								pos.u = current_kd.index;
							return_bcop->operand = pos;
						}
					}

					// pos.u = current_kd.pos + 1;
					pos.u = current_kd.index + 1;
					bytecode_operation* end_bcop = sabr_compiler_get_bcop(bc_data, first_kd->index);
					if (!end_bcop) goto FREE_ALL;
					end_bcop->operand = pos;

					if (!sabr_bytecode_write_bcop(bc_data, OP_RETURN)) goto FREE_ALL;
					if (!sabr_bytecode_write_bcop(bc_data, OP_DEFINE)) goto FREE_ALL;

					free_func_vecs();
					#undef free_func_vecs
				} break;
				case KWRD_STRUCT:
					for (size_t i = 1; i < temp_kd_vec->size; i++) {
						keyword_data* iter_kd = vector_at(keyword_data, temp_kd_vec, i);
						switch (iter_kd->kwrd) {
							case KWRD_RETURN: break;
							default: goto FAILURE_WRONG;
						}
					}
					if (!sabr_bytecode_write_bcop(bc_data, OP_STRUCT_END)) goto FREE_ALL;
					break;
				default: goto FAILURE_WRONG;
			}
			vector_free(keyword_data, temp_kd_vec);
			if (!vector_pop_back(cctl_ptr(vector(keyword_data)), &comp->keyword_data_stack)) goto FREE_ALL;
		} break;
		case KWRD_BRACKET_BEGIN:
			temp_kd_vec = (vector(keyword_data)*) malloc(sizeof(vector(keyword_data)));
			if (!temp_kd_vec) goto FAILURE_ALLOC;
			vector_init(keyword_data, temp_kd_vec);
			if (!vector_push_back(keyword_data, temp_kd_vec, current_kd)) goto FAILURE_ALLOC;
			if (!vector_push_back(cctl_ptr(vector(keyword_data)), &comp->keyword_data_stack, temp_kd_vec))
				goto FAILURE_ALLOC;
			if (!sabr_bytecode_write_bcop(bc_data, OP_ARRAY)) goto FREE_ALL;
			break;
		case KWRD_COMMA:
			if (!comp->keyword_data_stack.size) goto FAILURE_WRONG;
			temp_kd_vec = *vector_back(cctl_ptr(vector(keyword_data)), &comp->keyword_data_stack);
			if (!vector_push_back(keyword_data, temp_kd_vec, current_kd)) goto FAILURE_ALLOC;
			if (!sabr_bytecode_write_bcop(bc_data, OP_ARRAY_COMMA)) goto FREE_ALL;
			break;
		case KWRD_BRACKET_END:
			if (comp->keyword_data_stack.size == 0) goto FAILURE_WRONG;
			temp_kd_vec = *vector_back(cctl_ptr(vector(keyword_data)), &comp->keyword_data_stack);
			keyword_data* first_kd = vector_front(keyword_data, temp_kd_vec);

			switch (first_kd->kwrd) {
				case KWRD_BRACKET_BEGIN: {
					for (size_t i = 1; i < temp_kd_vec->size; i++) {
						keyword_data* iter_kd = vector_at(keyword_data, temp_kd_vec, i);
						switch (iter_kd->kwrd) {
							case KWRD_COMMA: break;
							default: goto FAILURE_WRONG;
						}
					}
				} break;
				default: goto FAILURE_WRONG;
			}
			if (!sabr_bytecode_write_bcop(bc_data, OP_ARRAY_END)) goto FREE_ALL;
			vector_free(keyword_data, temp_kd_vec);
			if (!vector_pop_back(cctl_ptr(vector(keyword_data)), &comp->keyword_data_stack)) goto FREE_ALL;
			
			break;
		default:
			break;
	}
	return true;

FAILURE_ALLOC:
	fputs(sabr_errmsg_alloc, stderr); 
	goto FREE_ALL;

FAILURE_WRONG:
	fputs(sabr_errmsg_wrong_keyword_syntax, stderr); 
	goto FREE_ALL;

FREE_ALL:
	if (temp_kd_vec) {
		vector_free(keyword_data, temp_kd_vec);
		free(temp_kd_vec);
	}

	return false;
}

bytecode_operation* sabr_compiler_get_bcop(bytecode* bc_data, size_t index) {
	if (index >= bc_data->bcop_vec.size) return NULL;
	return vector_at(bytecode_operation, &bc_data->bcop_vec, index);
}