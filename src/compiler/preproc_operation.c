#include "preproc_operation.h"

const bool sabr_compiler_preproc_def_base(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens, bool is_local, bool is_func) {
	sabr_token_t code_token = {0, };
	sabr_token_t identifier_token = {0, };

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	code_token = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	identifier_token = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	if (identifier_token.data[0] != '$') {
		fputs(sabr_errmsg_invalid_ident_fmt, stderr); goto FREE_ALL;
	}

	sabr_token_t macro_code_token = code_token;
	macro_code_token.data = sabr_new_string_copy(code_token.data);

	sabr_preproc_def_data_t def_data;
	def_data.def_code = macro_code_token;
	def_data.is_func = is_func;

	sabr_word_t macro_word;
	macro_word.type = SABR_WT_PREPROC_IDFR;
	macro_word.data.preproc_def_data = def_data;

	trie(sabr_word_t)* dictionary = (
		is_local
		? *vector_back(cctl_ptr(trie(sabr_word_t)), &comp->preproc_local_dictionary_stack)
		: &comp->preproc_dictionary
	);
	sabr_word_t* identifier_word = trie_find(sabr_word_t, dictionary, identifier_token.data + 1);
	if (identifier_word) {
		sabr_token_t prev_macro_code_token = identifier_word->data.preproc_def_data.def_code;
		free(prev_macro_code_token.data);
		identifier_word->data.preproc_def_data = def_data;
	}
	else {
		if (!trie_insert(sabr_word_t, dictionary, identifier_token.data + 1, macro_word)) {
			fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
		}
	}

	result = true;
FREE_ALL:
	if (!result) {
		free(macro_code_token.data);
	}
	free(code_token.data);
	free(identifier_token.data);
	return result;
}

const bool sabr_compiler_preproc_isdef_base(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens, bool is_local) {
	sabr_token_t identifier_token = {0, };
	sabr_token_t result_token = {0, };

	bool result = false;

	if (output_tokens->size < 1) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	identifier_token = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	if (identifier_token.data[0] != '$') {
		fputs(sabr_errmsg_invalid_ident_fmt, stderr); goto FREE_ALL;
	}
	
	trie(sabr_word_t)* dictionary = (
		is_local
		? *vector_back(cctl_ptr(trie(sabr_word_t)), &comp->preproc_local_dictionary_stack)
		: &comp->preproc_dictionary
	);
	sabr_word_t* identifier_word = trie_find(sabr_word_t, dictionary, identifier_token.data + 1);
	int flag = identifier_word ? ((identifier_word->type == SABR_WT_PREPROC_IDFR) ? 1 : 0) : 0;

	result_token = t;
	if (asprintf(&(result_token.data), "%d", flag) == -1) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}
	result_token.is_generated = true;

	if (!vector_push_back(sabr_token_t, output_tokens, result_token)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	result = true;
FREE_ALL:
	if (!result) {
		free(result_token.data);
	}
	free(identifier_token.data);
	return result;
}

const bool sabr_compiler_preproc_undef_base(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens, bool is_local) {
	sabr_token_t identifier_token = {0, };

	bool result = false;

	if (output_tokens->size < 1) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	identifier_token = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	if (identifier_token.data[0] != '$') {
		fputs(sabr_errmsg_invalid_ident_fmt, stderr); goto FREE_ALL;
	}

	trie(sabr_word_t)* dictionary = (
		is_local
		? *vector_back(cctl_ptr(trie(sabr_word_t)), &comp->preproc_local_dictionary_stack)
		: &comp->preproc_dictionary
	);
	trie_remove(sabr_word_t, dictionary, identifier_token.data + 1);

	result = true;
FREE_ALL:
	free(identifier_token.data);
	return result;
}

const bool sabr_compiler_preproc_getdef_base(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens, bool is_local) {
	sabr_token_t identifier_token = {0, };
	sabr_token_t result_token = {0, };

	bool result = false;

	if (output_tokens->size < 1) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	identifier_token = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	if (identifier_token.data[0] != '$') {
		fputs(sabr_errmsg_invalid_ident_fmt, stderr); goto FREE_ALL;
	}
	
	result_token = t;
	result_token.is_generated = false;

	trie(sabr_word_t)* dictionary = (
		is_local
		? *vector_back(cctl_ptr(trie(sabr_word_t)), &comp->preproc_local_dictionary_stack)
		: &comp->preproc_dictionary
	);
	sabr_word_t* identifier_word = trie_find(sabr_word_t, dictionary, identifier_token.data + 1);
	if (identifier_word) {
		sabr_token_t macro_code = identifier_word->data.preproc_def_data.def_code;
		result_token.data = sabr_new_string_copy(macro_code.data);
	}
	else {
		result_token.data = sabr_new_string_copy("{}");
	}
	if (!result_token.data) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, result_token)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	result = true;
FREE_ALL:
	if (!result) {
		free(result_token.data);
	}
	free(identifier_token.data);
	return result;
}

extern const bool sabr_compiler_preproc_func(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens);
extern const bool sabr_compiler_preproc_macro(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens);
extern const bool sabr_compiler_preproc_isdef(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens);
extern const bool sabr_compiler_preproc_undef(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens);
extern const bool sabr_compiler_preproc_getdef(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens);
extern const bool sabr_compiler_preproc_lfunc(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens);
extern const bool sabr_compiler_preproc_lmacro(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens);
extern const bool sabr_compiler_preproc_lisdef(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens);
extern const bool sabr_compiler_preproc_lundef(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens);
extern const bool sabr_compiler_preproc_lgetdef(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens);

const bool sabr_compiler_preproc_import(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_token_t filename_token = {0, };
	char filename_full[PATH_MAX] = {0, };
	size_t textcode_index;
	char* filename = NULL;
	char* current_filename = NULL;

	bool local_file = false;
	vector(sabr_token_t)* preprocessed_tokens = NULL;

	bool result = false;

	if (output_tokens->size < 1) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	filename_token = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	filename = filename_token.data;
	local_file = (*filename == ':');
	if (local_file) filename++;

	current_filename = *vector_at(cctl_ptr(char), &comp->filename_vector, t.textcode_index);

	char import_filename[PATH_MAX];

#if defined(_WIN32)

	if (local_file) sabr_get_local_file_path(import_filename, current_filename, filename, true, &comp->convert_state);
	else if (!sabr_get_std_lib_path(import_filename, filename, true, &comp->convert_state)) {
		fputs(sabr_errmsg_fullpath, stderr); goto FREE_ALL;
	}
#else
	if (local_file) sabr_get_local_file_path(import_filename, current_filename, filename, true);
	else if (!sabr_get_std_lib_path(import_filename, filename, true)) {
		fputs(sabr_errmsg_fullpath, stderr); goto FREE_ALL;
	}
#endif

#if defined(_WIN32)
	if (!_fullpath(filename_full, import_filename, PATH_MAX)) {
		fputs(sabr_errmsg_fullpath, stderr); goto FREE_ALL;
	}
#else
	if (!(realpath(import_filename, filename_full))) {
		fputs(sabr_errmsg_fullpath, stderr); goto FREE_ALL;
	}
#endif

	if (!trie_find(size_t, &comp->filename_trie, filename_full)) {
		if (!sabr_compiler_load_file(comp, filename_full, &textcode_index)) goto FREE_ALL;

		preprocessed_tokens = sabr_compiler_preprocess_textcode(comp, textcode_index);
		if (!preprocessed_tokens) goto FREE_ALL;

		for (size_t i = 0; i < preprocessed_tokens->size; i++) {
			if (!vector_push_back(sabr_token_t, output_tokens, *vector_at(sabr_token_t, preprocessed_tokens, i))) {
				fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
			}
		}
	}

	result = true;
FREE_ALL:
	if (!result) {
		sabr_free_token_vector(preprocessed_tokens);
		preprocessed_tokens = NULL;
	}
	vector_free(sabr_token_t, preprocessed_tokens);
	free(preprocessed_tokens);
	free(filename_token.data);
	return result;
}

const bool sabr_compiler_preproc_eval(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_token_t code_token = {0, };
	sabr_token_t result_token = {0, };

	bool result = false;

	if (output_tokens->size < 1) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	code_token = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	if (code_token.data[0] == '{') {
		output_tokens = sabr_compiler_preprocess_eval_token(comp, code_token, false, output_tokens);
		if (!output_tokens) goto FREE_ALL;
	}
	else {
		result_token = code_token;
		result_token.data = sabr_new_string_copy(code_token.data);
		if (!result_token.data) {
			fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
		}
		if (!vector_push_back(sabr_token_t, output_tokens, result_token)) {
			fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
		}
	}

	result = true;
FREE_ALL:
	if (!result) {
		free(result_token.data);
	}
	free(code_token.data);
	return result;
}

const bool sabr_compiler_preproc_if(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_token_t code_token = {0, };
	sabr_token_t code_token_a = {0, };
	sabr_token_t code_token_b = {0, };
	sabr_token_t flag_token = {0, };
	sabr_value_t flag_value;
	sabr_token_t result_token = {0, };

	bool result = false;

	if (output_tokens->size < 3) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	code_token_b = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	code_token_a = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	flag_token = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	if (!sabr_compiler_preprocess_parse_value(comp, flag_token, &flag_value)) goto FREE_ALL;

	code_token = flag_value.u ? code_token_a : code_token_b;

	if (code_token.data[0] == '{') {
		output_tokens = sabr_compiler_preprocess_eval_token(comp, code_token, false, output_tokens);
		if (!output_tokens) goto FREE_ALL;
	}
	else {
		result_token = code_token;
		result_token.data = sabr_new_string_copy(code_token.data);
		if (!result_token.data) {
			fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
		}
		if (!vector_push_back(sabr_token_t, output_tokens, result_token)) {
			fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
		}
	}

	result = true;
FREE_ALL:
	if (!result) {
		free(result_token.data);
	}
	free(code_token_a.data);
	free(code_token_b.data);
	free(flag_token.data);
	return result;
}

const bool sabr_compiler_preproc_while(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_token_t code_token = {0, };
	sabr_token_t flag_code_token = {0, };
	sabr_token_t flag_token = {0, };
	sabr_value_t flag_value;
	sabr_token_t result_token = {0, };

	bool is_flag_code_token_brace = false;
	bool is_code_token_brace = false;

	sabr_preproc_stop_flag_t* current_preproc_stop = NULL;

	bool result = false;
	bool loop_break = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	code_token = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	flag_code_token = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	is_flag_code_token_brace = flag_code_token.data[0] == '{';

	if (!is_flag_code_token_brace) {
		if (!sabr_compiler_preprocess_parse_value(comp, flag_code_token, &flag_value)) goto FREE_ALL;
		flag_value.u++;
	}

	is_code_token_brace = code_token.data[0] == '{';

	if (!vector_push_back(sabr_preproc_stop_flag_t, &comp->preproc_stop_stack, SABR_PPS_NONE)) {
		fputs(sabr_errmsg_alloc, stderr);
		goto FREE_ALL;
	}

	while (true) {
		if (is_flag_code_token_brace) {
			output_tokens = sabr_compiler_preprocess_eval_token(comp, flag_code_token, false, output_tokens);
			if (!output_tokens) goto FREE_ALL;
			flag_token = *vector_back(sabr_token_t, output_tokens);
			if (!vector_pop_back(sabr_token_t, output_tokens)) {
				fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
			}
			if (!sabr_compiler_preprocess_parse_value(comp, flag_token, &flag_value)) goto FREE_ALL;
		}
		else {
			flag_value.u--;
		}
		if (!flag_value.u) break;

		if (is_code_token_brace) {
			current_preproc_stop = vector_back(sabr_preproc_stop_flag_t, &comp->preproc_stop_stack);
			*current_preproc_stop = SABR_PPS_LOOP;
			output_tokens = sabr_compiler_preprocess_eval_token(comp, code_token, false, output_tokens);
			if (!output_tokens) goto FREE_ALL;
		}
		else {
			result_token = code_token;
			result_token.data = sabr_new_string_copy(code_token.data);
			if (!result_token.data) {
				fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
			}
			if (!vector_push_back(sabr_token_t, output_tokens, result_token)) {
				fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
			}
		}

		if (current_preproc_stop) {
			switch (*current_preproc_stop) {
				case SABR_PPS_BREAK:
					loop_break = true;
					break;
				default:
					break;
			}
			*current_preproc_stop = SABR_PPS_NONE;
			if (loop_break) break;
		}

		if (is_flag_code_token_brace) {
			free(flag_token.data);
			if (!memset(&flag_token, 0, sizeof(sabr_token_t))) {
				fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
			}
		}
		if (is_code_token_brace) {
			free(result_token.data);
			if (!memset(&result_token, 0, sizeof(sabr_token_t))) {
				fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
			}
		}
	}

	result = true;
FREE_ALL:
	if (!result) {
		free(result_token.data);
	}
	free(code_token.data);
	free(flag_code_token.data);
	free(flag_token.data);
	vector_pop_back(sabr_preproc_stop_flag_t, &comp->preproc_stop_stack);
	return result;
}

const bool sabr_compiler_preproc_break(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_preproc_stop_flag_t* current_preproc_stop = vector_back(sabr_preproc_stop_flag_t, &comp->preproc_stop_stack);
	if (*current_preproc_stop != SABR_PPS_LOOP) {
		fputs(sabr_errmsg_wrong_break_continue, stderr);
		return false;
	}
	*current_preproc_stop = SABR_PPS_BREAK;
	return true;
}

const bool sabr_compiler_preproc_continue(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_preproc_stop_flag_t* current_preproc_stop = vector_back(sabr_preproc_stop_flag_t, &comp->preproc_stop_stack);
	if (*current_preproc_stop != SABR_PPS_LOOP) {
		fputs(sabr_errmsg_wrong_break_continue, stderr);
		return false;
	}
	*current_preproc_stop = SABR_PPS_CONTINUE;
	return true;
}

const bool sabr_compiler_preproc_concat(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_token_t text_token_a = {0, };
	sabr_token_t text_token_b = {0, };
	sabr_token_t result_token = {0, };
	char* text_str_a = NULL;
	char* text_str_b = NULL;
	char* text_str_temp = NULL;
	sabr_string_parse_mode_t token_a_str_parse = SABR_STR_PARSE_NONE;
	sabr_string_parse_mode_t token_b_str_parse = SABR_STR_PARSE_NONE;

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	text_token_b = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	text_token_a = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	char string_begin[2] = {0, };
	char string_end[2] = {0, };

	switch (text_token_a.data[0]) {
		case '{': token_a_str_parse = SABR_STR_PARSE_PREPROC; break;
		case '\'': token_a_str_parse = SABR_STR_PARSE_SINGLE; break;
		case '\"': token_a_str_parse = SABR_STR_PARSE_DOUBLE; break;
		default: break;
	}
	switch (text_token_b.data[0]) {
		case '{': token_b_str_parse = SABR_STR_PARSE_PREPROC; break;
		case '\'': token_b_str_parse = SABR_STR_PARSE_SINGLE; break;
		case '\"': token_b_str_parse = SABR_STR_PARSE_DOUBLE; break;
		default: break;
	}

	if (token_a_str_parse) {
		text_str_a = sabr_new_string_slice(text_token_a.data, 1, strlen(text_token_a.data) - 1);
		text_str_b = (token_a_str_parse == token_b_str_parse)
			? sabr_new_string_slice(text_token_b.data, 1, strlen(text_token_b.data) - 1)
			: sabr_new_string_copy(text_token_b.data);
		switch (token_a_str_parse) {
			case SABR_STR_PARSE_PREPROC: string_begin[0] = '{'; string_end[0] = '}'; break;
			case SABR_STR_PARSE_SINGLE: string_begin[0] = '\''; string_end[0] = '\''; break;
			case SABR_STR_PARSE_DOUBLE: string_begin[0] = '\"'; string_end[0] = '\"'; break;
			default: break;
		}
	}
	else {
		text_str_a = sabr_new_string_copy(text_token_a.data);
		text_str_b = (token_b_str_parse)
			? sabr_new_string_slice(text_token_b.data, 1, strlen(text_token_b.data) - 1)
			: sabr_new_string_copy(text_token_b.data);
		switch (token_b_str_parse) {
			case SABR_STR_PARSE_PREPROC: string_begin[0] = '{'; string_end[0] = '}'; break;
			case SABR_STR_PARSE_SINGLE: string_begin[0] = '\''; string_end[0] = '\''; break;
			case SABR_STR_PARSE_DOUBLE: string_begin[0] = '\"'; string_end[0] = '\"'; break;
			default: break;
		}
	}
	if (!text_str_a) { fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL; }
	if (!text_str_b) { fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL; }

	if (
		asprintf(
			&text_str_temp, "%s%s%s%s",
			string_begin,
			text_str_a, text_str_b,
			string_end
		) == -1
	) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}
	
	result_token = t;
	result_token.data = text_str_temp;
	result_token.is_generated = true;

	if (!vector_push_back(sabr_token_t, output_tokens, result_token)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	result = true;
FREE_ALL:
	if (!result) {
		free(text_str_temp);
	}
	free(text_str_a);
	free(text_str_b);
	free(text_token_a.data);
	free(text_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_substr(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_token_t text_token = {0, };
	sabr_token_t begin_token = {0, };
	sabr_token_t end_token = {0, };
	sabr_token_t result_token = {0, };
	sabr_value_t begin_value;
	sabr_value_t end_value;
	bool is_string;
	char* sliced_result_str = NULL;
	char* final_result_str = NULL;

	vector(size_t) index_list;
	vector_init(size_t, &index_list);

	bool result = false;

	if (output_tokens->size < 3) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	end_token = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	begin_token = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	text_token = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	if (!sabr_compiler_preprocess_parse_value(comp, begin_token, &begin_value)) goto FREE_ALL; 
	if (!sabr_compiler_preprocess_parse_value(comp, end_token, &end_value)) goto FREE_ALL;

	char string_begin[2] = {0, };
	char string_end[2] = {0, };
	switch (text_token.data[0]) {
		case '{':
			string_begin[0] = '{';
			string_end[0] = '}';
			is_string = true;
			break;
		case '\'':
		case '\"':
			string_begin[0] = text_token.data[0];
			string_end[0] = text_token.data[0];
			is_string = true;
			break;
		default: break;
	}

	size_t len_u8 = strlen(text_token.data);
	size_t len_u32 = 0;
	char* ch = text_token.data;

	while (*ch) {
		if (((signed char) *ch) >= -64) {
			vector_push_back(size_t, &index_list, ch - text_token.data);
			len_u32++;
		}
		ch++;
	}
	vector_push_back(size_t, &index_list, len_u8);

	size_t len = len_u32 - (is_string ? 2 : 0);

	if (begin_value.i < 0) { fputs(sabr_errmsg_out_of_index, stderr); goto FREE_ALL; }
	if (end_value.i < 0) { fputs(sabr_errmsg_out_of_index, stderr); goto FREE_ALL; }
	if (begin_value.i >= len) { fputs(sabr_errmsg_out_of_index, stderr); goto FREE_ALL; }
	if (end_value.i > len) { fputs(sabr_errmsg_out_of_index, stderr); goto FREE_ALL; }
	if (begin_value.i >= end_value.i) { fputs(sabr_errmsg_out_of_index, stderr); goto FREE_ALL; }

	size_t begin_index = *vector_at(size_t, &index_list, begin_value.i + (is_string ? 1 : 0));
	size_t end_index = *vector_at(size_t, &index_list, end_value.i + (is_string ? 1 : 0));

	sliced_result_str = sabr_new_string_slice(text_token.data, begin_index, end_index);
	if (!sliced_result_str) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (
		asprintf(
			&final_result_str, "%s%s%s",
			string_begin,
			sliced_result_str,
			string_end
		) == -1
	) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	result_token = t;
	result_token.data = final_result_str;
	result_token.is_generated = true;

	if (!vector_push_back(sabr_token_t, output_tokens, result_token)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	result = true;
FREE_ALL:
	if (!result) {
		free(final_result_str);
	}
	free(sliced_result_str);
	free(text_token.data);
	free(begin_token.data);
	free(end_token.data);

	vector_free(size_t, &index_list);

	return result;
}

const bool sabr_compiler_preproc_trim(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_token_t text_token = {0, };
	sabr_token_t result_token = {0, };
	char* sliced_result_str = NULL;
	char* final_result_str = NULL;
	bool is_string = false;

	bool result = false;

	if (output_tokens->size < 1) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	text_token = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	size_t begin_index = 0;
	size_t end_index = strlen(text_token.data);

	char string_begin[2] = {0, };
	char string_end[2] = {0, };
	switch (text_token.data[0]) {
		case '{':
			string_begin[0] = '{';
			string_end[0] = '}';
			is_string = true;
			break;
		case '\'':
		case '\"':
			string_begin[0] = text_token.data[0];
			string_end[0] = text_token.data[0];
			is_string = true;
			break;
		default: break;
	}

	char* ch = text_token.data;
	if (is_string) { ch++; begin_index++; }
	while (*ch && isspace(*ch)) { ch++; begin_index++; }
	
	ch = text_token.data + end_index - 1;
	if (is_string) { ch--; end_index--; }
	while (ch >= text_token.data + begin_index && isspace(*ch)) { ch--; end_index--; }

	sliced_result_str = sabr_new_string_slice(text_token.data, begin_index, end_index);
	if (!sliced_result_str) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (
		asprintf(
			&final_result_str, "%s%s%s",
			string_begin,
			sliced_result_str,
			string_end
		) == -1
	) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	result_token = t;
	result_token.data = final_result_str;
	result_token.is_generated = true;

	if (!vector_push_back(sabr_token_t, output_tokens, result_token)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	result = true;
FREE_ALL:
	free(text_token.data);

	return result;
}

const bool sabr_compiler_preproc_compare(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_token_t text_token_a = {0, };
	sabr_token_t text_token_b = {0, };
	sabr_token_t result_token = {0, };
	char* result_str = NULL;

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	text_token_b = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	text_token_a = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	if (asprintf(&result_str, "%d", strcmp(text_token_a.data, text_token_b.data)) == -1) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	result_token = t;
	result_token.data = result_str;
	result_token.is_generated = true;

	if (!vector_push_back(sabr_token_t, output_tokens, result_token)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	result = true;
FREE_ALL:
	if (!result) {
		free(result_str);
	}
	free(text_token_a.data);
	free(text_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_len(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_token_t text_token = {0, };
	sabr_token_t result_token = {0, };
	char* result_str = NULL;
	bool is_string;

	bool result = false;

	if (output_tokens->size < 1) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	text_token = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	switch (text_token.data[0]) {
		case '{':
		case '\'':
		case '\"':
			is_string = true;
			break;
		default: break;
	}

	size_t len_u32 = 0;
	char* ch = text_token.data;
	while (*ch) {
		if (((signed char) *ch) >= -64) {
			len_u32++;
		}
		ch++;
	}

	if (asprintf(&result_str, "%zu", len_u32 - (is_string ? 2 : 0)) == -1) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	result_token = t;
	result_token.data = result_str;
	result_token.is_generated = true;

	if (!vector_push_back(sabr_token_t, output_tokens, result_token)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	result = true;
FREE_ALL:
	if (!result) {
		free(result_str);
	}
	free(text_token.data);
	return result;
}

const bool sabr_compiler_preproc_drop(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_token_t value_token_a = {0, };

	bool result = false;

	if (output_tokens->size < 1) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_a = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	result = true;
FREE_ALL:
	free(value_token_a.data);
	return result;
}

const bool sabr_compiler_preproc_nip(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_token_t value_token_a = {0, };
	sabr_token_t value_token_b = {0, };

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_b = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_a = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, value_token_b)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	result = true;
FREE_ALL:
	if (!result) {
		free(value_token_b.data);
	}
	free(value_token_a.data);
	return result;
}

const bool sabr_compiler_preproc_dup(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_token_t value_token_a = {0, };
	sabr_token_t value_token_a2 = {0, };

	bool result = false;

	if (output_tokens->size < 1) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_a = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	value_token_a2 = value_token_a;
	value_token_a2.data = sabr_new_string_copy(value_token_a.data);
	if (!value_token_a2.data) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, value_token_a)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, value_token_a2)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	result = true;
FREE_ALL:
	if (!result) {
		free(value_token_a.data);
		free(value_token_a2.data);
	}
	return result;
}

const bool sabr_compiler_preproc_over(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_token_t value_token_a = {0, };
	sabr_token_t value_token_a2 = {0, };
	sabr_token_t value_token_b = {0, };

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_b = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_a = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	value_token_a2 = value_token_a;
	value_token_a2.data = sabr_new_string_copy(value_token_a.data);
	if (!value_token_a2.data) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, value_token_a)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, value_token_b)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, value_token_a2)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	result = true;
FREE_ALL:
	if (!result) {
		free(value_token_a.data);
		free(value_token_a2.data);
		free(value_token_b.data);
	}
	return result;
}

const bool sabr_compiler_preproc_tuck(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_token_t value_token_a = {0, };
	sabr_token_t value_token_b = {0, };
	sabr_token_t value_token_b2 = {0, };

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_b = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_a = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	value_token_b2 = value_token_a;
	value_token_b2.data = sabr_new_string_copy(value_token_b.data);
	if (!value_token_b2.data) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, value_token_b)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, value_token_a)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, value_token_b2)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	result = true;
FREE_ALL:
	if (!result) {
		free(value_token_a.data);
		free(value_token_b.data);
		free(value_token_b2.data);
	}
	return result;
}

const bool sabr_compiler_preproc_swap(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_token_t value_token_a = {0, };
	sabr_token_t value_token_b = {0, };

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_b = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_a = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, value_token_b)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, value_token_a)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	result = true;
FREE_ALL:
	if (!result) {
		free(value_token_a.data);
		free(value_token_b.data);
	}
	return result;
}

const bool sabr_compiler_preproc_rot(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_token_t value_token_a = {0, };
	sabr_token_t value_token_b = {0, };
	sabr_token_t value_token_c = {0, };

	bool result = false;

	if (output_tokens->size < 3) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_c = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_b = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_a = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, value_token_b)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, value_token_c)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, value_token_a)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	result = true;
FREE_ALL:
	if (!result) {
		free(value_token_a.data);
		free(value_token_b.data);
		free(value_token_c.data);
	}
	return result;
}

const bool sabr_compiler_preproc_2drop(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_token_t value_token_a = {0, };
	sabr_token_t value_token_b = {0, };

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_b = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_a = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	result = true;
FREE_ALL:
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_2nip(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_token_t value_token_a = {0, };
	sabr_token_t value_token_b = {0, };
	sabr_token_t value_token_c = {0, };
	sabr_token_t value_token_d = {0, };

	bool result = false;

	if (output_tokens->size < 4) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_d = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_c = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_b = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_a = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, value_token_c)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, value_token_d)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	result = true;
FREE_ALL:
	if (!result) {
		free(value_token_c.data);
		free(value_token_d.data);
	}

	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_2dup(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_token_t value_token_a = {0, };
	sabr_token_t value_token_a2 = {0, };
	sabr_token_t value_token_b = {0, };
	sabr_token_t value_token_b2 = {0, };

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_b = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_a = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	value_token_a2 = value_token_a;
	value_token_a2.data = sabr_new_string_copy(value_token_a.data);
	if (!value_token_a2.data) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	value_token_b2 = value_token_b;
	value_token_b2.data = sabr_new_string_copy(value_token_b.data);
	if (!value_token_b2.data) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, value_token_a)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, value_token_b)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, value_token_a2)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, value_token_b2)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	result = true;
FREE_ALL:
	if (!result) {
		free(value_token_a.data);
		free(value_token_a2.data);
		free(value_token_b.data);
		free(value_token_b2.data);
	}

	return result;
}

const bool sabr_compiler_preproc_2over(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_token_t value_token_a = {0, };
	sabr_token_t value_token_a2 = {0, };
	sabr_token_t value_token_b = {0, };
	sabr_token_t value_token_b2 = {0, };
	sabr_token_t value_token_c = {0, };
	sabr_token_t value_token_d = {0, };

	bool result = false;

	if (output_tokens->size < 4) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_d = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_c = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_b = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_a = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	value_token_a2 = value_token_a;
	value_token_a2.data = sabr_new_string_copy(value_token_a.data);
	if (!value_token_a2.data) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	value_token_b2 = value_token_b;
	value_token_b2.data = sabr_new_string_copy(value_token_b.data);
	if (!value_token_b2.data) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, value_token_a)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, value_token_b)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, value_token_c)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, value_token_d)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, value_token_a2)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, value_token_b2)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	result = true;
FREE_ALL:
	if (!result) {
		free(value_token_a.data);
		free(value_token_a2.data);
		free(value_token_b.data);
		free(value_token_b2.data);
		free(value_token_c.data);
		free(value_token_d.data);
	}
	return result;
}

const bool sabr_compiler_preproc_2tuck(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_token_t value_token_a = {0, };
	sabr_token_t value_token_b = {0, };
	sabr_token_t value_token_c = {0, };
	sabr_token_t value_token_c2 = {0, };
	sabr_token_t value_token_d = {0, };
	sabr_token_t value_token_d2 = {0, };

	bool result = false;

	if (output_tokens->size < 4) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_d = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_c = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_b = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_a = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	value_token_c2 = value_token_c;
	value_token_c2.data = sabr_new_string_copy(value_token_c.data);
	if (!value_token_c2.data) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	value_token_d2 = value_token_d;
	value_token_d2.data = sabr_new_string_copy(value_token_d.data);
	if (!value_token_d2.data) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, value_token_c)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, value_token_d)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, value_token_a)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, value_token_b)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, value_token_c2)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, value_token_d2)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	result = true;
FREE_ALL:
	if (!result) {
		free(value_token_a.data);
		free(value_token_b.data);
		free(value_token_c.data);
		free(value_token_c2.data);
		free(value_token_d.data);
		free(value_token_d2.data);
	}
	return result;
}

const bool sabr_compiler_preproc_2swap(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_token_t value_token_a = {0, };
	sabr_token_t value_token_b = {0, };
	sabr_token_t value_token_c = {0, };
	sabr_token_t value_token_d = {0, };

	bool result = false;

	if (output_tokens->size < 4) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_d = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_c = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_b = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_a = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, value_token_c)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, value_token_d)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, value_token_a)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, value_token_b)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	result = true;
FREE_ALL:
	if (!result) {
		free(value_token_a.data);
		free(value_token_b.data);
		free(value_token_c.data);
		free(value_token_d.data);
	}
	return result;
}

const bool sabr_compiler_preproc_2rot(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_token_t value_token_a = {0, };
	sabr_token_t value_token_b = {0, };
	sabr_token_t value_token_c = {0, };
	sabr_token_t value_token_d = {0, };
	sabr_token_t value_token_e = {0, };
	sabr_token_t value_token_f = {0, };

	bool result = false;

	if (output_tokens->size < 6) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_f = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_e = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_d = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_c = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_b = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_a = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, value_token_c)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, value_token_d)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, value_token_e)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, value_token_f)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, value_token_a)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, value_token_b)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	result = true;
FREE_ALL:
	if (!result) {
		free(value_token_a.data);
		free(value_token_b.data);
		free(value_token_c.data);
		free(value_token_d.data);
		free(value_token_e.data);
		free(value_token_f.data);
	}
	return result;
}

const bool sabr_compiler_preproc_add(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_token_t value_token_a = {0, };
	sabr_token_t value_token_b = {0, };
	sabr_token_t result_token = {0, };
	sabr_value_t value_a;
	sabr_value_t value_b;
	sabr_value_t result_value;

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_b = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_a = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_a, &value_a)) goto FREE_ALL;

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_b, &value_b)) goto FREE_ALL;

	result_value.i = value_a.i + value_b.i;

	result_token = t;
	result_token.is_generated = true;
	
	if (asprintf(&(result_token.data), "%" PRId64, result_value.i) == -1) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, result_token)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	result = true;
FREE_ALL:
	if (!result) {
		free(result_token.data);
	}
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_sub(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_token_t value_token_a = {0, };
	sabr_token_t value_token_b = {0, };
	sabr_token_t result_token = {0, };
	sabr_value_t value_a;
	sabr_value_t value_b;
	sabr_value_t result_value;

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_b = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_a = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_a, &value_a)) goto FREE_ALL;

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_b, &value_b)) goto FREE_ALL;

	result_value.i = value_a.i - value_b.i;

	result_token = t;
	result_token.is_generated = true;
	
	if (asprintf(&(result_token.data), "%" PRId64, result_value.i) == -1) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, result_token)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	result = true;
FREE_ALL:
	if (!result) {
		free(result_token.data);
	}
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_mul(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_token_t value_token_a = {0, };
	sabr_token_t value_token_b = {0, };
	sabr_token_t result_token = {0, };
	sabr_value_t value_a;
	sabr_value_t value_b;
	sabr_value_t result_value;

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_b = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_a = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_a, &value_a)) goto FREE_ALL;

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_b, &value_b)) goto FREE_ALL;

	result_value.i = value_a.i * value_b.i;

	result_token = t;
	result_token.is_generated = true;
	
	if (asprintf(&(result_token.data), "%" PRId64, result_value.i) == -1) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, result_token)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	result = true;
FREE_ALL:
	if (!result) {
		free(result_token.data);
	}
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_div(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_token_t value_token_a = {0, };
	sabr_token_t value_token_b = {0, };
	sabr_token_t result_token = {0, };
	sabr_value_t value_a;
	sabr_value_t value_b;
	sabr_value_t result_value;

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_b = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_a = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_a, &value_a)) goto FREE_ALL;

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_b, &value_b)) goto FREE_ALL;

	if (value_b.i == 0) {
		fputs(sabr_errmsg_div_zero, stderr); goto FREE_ALL;
	}

	result_value.i = value_a.i / value_b.i;

	result_token = t;
	result_token.is_generated = true;
	
	if (asprintf(&(result_token.data), "%" PRId64, result_value.i) == -1) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, result_token)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	result = true;
FREE_ALL:
	if (!result) {
		free(result_token.data);
	}
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_mod(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_token_t value_token_a = {0, };
	sabr_token_t value_token_b = {0, };
	sabr_token_t result_token = {0, };
	sabr_value_t value_a;
	sabr_value_t value_b;
	sabr_value_t result_value;

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_b = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_a = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_a, &value_a)) goto FREE_ALL;

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_b, &value_b)) goto FREE_ALL;

	if (value_b.i == 0) {
		fputs(sabr_errmsg_div_zero, stderr); goto FREE_ALL;
	}

	result_value.i = value_a.i % value_b.i;

	result_token = t;
	result_token.is_generated = true;
	
	if (asprintf(&(result_token.data), "%" PRId64, result_value.i) == -1) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, result_token)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	result = true;
FREE_ALL:
	if (!result) {
		free(result_token.data);
	}
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_udiv(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_token_t value_token_a = {0, };
	sabr_token_t value_token_b = {0, };
	sabr_token_t result_token = {0, };
	sabr_value_t value_a;
	sabr_value_t value_b;
	sabr_value_t result_value;

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_b = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_a = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_a, &value_a)) goto FREE_ALL;

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_b, &value_b)) goto FREE_ALL;

	if (value_b.u == 0) {
		fputs(sabr_errmsg_div_zero, stderr); goto FREE_ALL;
	}

	result_value.u = value_a.u / value_b.u;

	result_token = t;
	result_token.is_generated = true;
	
	if (asprintf(&(result_token.data), "%" PRIu64, result_value.u) == -1) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, result_token)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	result = true;
FREE_ALL:
	if (!result) {
		free(result_token.data);
	}
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_umod(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_token_t value_token_a = {0, };
	sabr_token_t value_token_b = {0, };
	sabr_token_t result_token = {0, };
	sabr_value_t value_a;
	sabr_value_t value_b;
	sabr_value_t result_value;

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_b = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_a = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_a, &value_a)) goto FREE_ALL;

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_b, &value_b)) goto FREE_ALL;

	if (value_b.u == 0) {
		fputs(sabr_errmsg_div_zero, stderr); goto FREE_ALL;
	}

	result_value.u = value_a.u % value_b.u;

	result_token = t;
	result_token.is_generated = true;
	
	if (asprintf(&(result_token.data), "%" PRIu64, result_value.u) == -1) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, result_token)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	result = true;
FREE_ALL:
	if (!result) {
		free(result_token.data);
	}
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_equ(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_token_t value_token_a = {0, };
	sabr_token_t value_token_b = {0, };
	sabr_token_t result_token = {0, };
	sabr_value_t value_a;
	sabr_value_t value_b;
	sabr_value_t result_value;

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_b = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_a = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_a, &value_a)) goto FREE_ALL;

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_b, &value_b)) goto FREE_ALL;

	result_value.i = (value_a.i == value_b.i) ? 1 : 0;

	result_token = t;
	result_token.is_generated = true;
	
	if (asprintf(&(result_token.data), "%" PRId64, result_value.i) == -1) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, result_token)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	result = true;
FREE_ALL:
	if (!result) {
		free(result_token.data);
	}
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_neq(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_token_t value_token_a = {0, };
	sabr_token_t value_token_b = {0, };
	sabr_token_t result_token = {0, };
	sabr_value_t value_a;
	sabr_value_t value_b;
	sabr_value_t result_value;

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_b = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_a = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_a, &value_a)) goto FREE_ALL;

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_b, &value_b)) goto FREE_ALL;

	result_value.i = (value_a.i != value_b.i) ? 1 : 0;

	result_token = t;
	result_token.is_generated = true;
	
	if (asprintf(&(result_token.data), "%" PRId64, result_value.i) == -1) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, result_token)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	result = true;
FREE_ALL:
	if (!result) {
		free(result_token.data);
	}
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_grt(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_token_t value_token_a = {0, };
	sabr_token_t value_token_b = {0, };
	sabr_token_t result_token = {0, };
	sabr_value_t value_a;
	sabr_value_t value_b;
	sabr_value_t result_value;

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_b = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_a = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_a, &value_a)) goto FREE_ALL;

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_b, &value_b)) goto FREE_ALL;

	result_value.i = (value_a.i < value_b.i) ? 1 : 0;

	result_token = t;
	result_token.is_generated = true;
	
	if (asprintf(&(result_token.data), "%" PRId64, result_value.i) == -1) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, result_token)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	result = true;
FREE_ALL:
	if (!result) {
		free(result_token.data);
	}
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_geq(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_token_t value_token_a = {0, };
	sabr_token_t value_token_b = {0, };
	sabr_token_t result_token = {0, };
	sabr_value_t value_a;
	sabr_value_t value_b;
	sabr_value_t result_value;

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_b = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_a = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_a, &value_a)) goto FREE_ALL;

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_b, &value_b)) goto FREE_ALL;

	result_value.i = (value_a.i <= value_b.i) ? 1 : 0;

	result_token = t;
	result_token.is_generated = true;
	
	if (asprintf(&(result_token.data), "%" PRId64, result_value.i) == -1) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, result_token)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	result = true;
FREE_ALL:
	if (!result) {
		free(result_token.data);
	}
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_lst(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_token_t value_token_a = {0, };
	sabr_token_t value_token_b = {0, };
	sabr_token_t result_token = {0, };
	sabr_value_t value_a;
	sabr_value_t value_b;
	sabr_value_t result_value;

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_b = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_a = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_a, &value_a)) goto FREE_ALL;

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_b, &value_b)) goto FREE_ALL;

	result_value.i = (value_a.i > value_b.i) ? 1 : 0;

	result_token = t;
	result_token.is_generated = true;
	
	if (asprintf(&(result_token.data), "%" PRId64, result_value.i) == -1) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, result_token)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	result = true;
FREE_ALL:
	if (!result) {
		free(result_token.data);
	}
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_leq(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_token_t value_token_a = {0, };
	sabr_token_t value_token_b = {0, };
	sabr_token_t result_token = {0, };
	sabr_value_t value_a;
	sabr_value_t value_b;
	sabr_value_t result_value;

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_b = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_a = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_a, &value_a)) goto FREE_ALL;

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_b, &value_b)) goto FREE_ALL;

	result_value.i = (value_a.i >= value_b.i) ? 1 : 0;

	result_token = t;
	result_token.is_generated = true;
	
	if (asprintf(&(result_token.data), "%" PRId64, result_value.i) == -1) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, result_token)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	result = true;
FREE_ALL:
	if (!result) {
		free(result_token.data);
	}
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_ugrt(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_token_t value_token_a = {0, };
	sabr_token_t value_token_b = {0, };
	sabr_token_t result_token = {0, };
	sabr_value_t value_a;
	sabr_value_t value_b;
	sabr_value_t result_value;

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_b = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_a = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_a, &value_a)) goto FREE_ALL;

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_b, &value_b)) goto FREE_ALL;

	result_value.i = (value_a.u < value_b.u) ? 1 : 0;

	result_token = t;
	result_token.is_generated = true;
	
	if (asprintf(&(result_token.data), "%" PRId64, result_value.i) == -1) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, result_token)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	result = true;
FREE_ALL:
	if (!result) {
		free(result_token.data);
	}
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_ugeq(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_token_t value_token_a = {0, };
	sabr_token_t value_token_b = {0, };
	sabr_token_t result_token = {0, };
	sabr_value_t value_a;
	sabr_value_t value_b;
	sabr_value_t result_value;

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_b = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_a = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_a, &value_a)) goto FREE_ALL;

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_b, &value_b)) goto FREE_ALL;

	result_value.i = (value_a.u <= value_b.u) ? 1 : 0;

	result_token = t;
	result_token.is_generated = true;
	
	if (asprintf(&(result_token.data), "%" PRId64, result_value.i) == -1) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, result_token)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	result = true;
FREE_ALL:
	if (!result) {
		free(result_token.data);
	}
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_ulst(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_token_t value_token_a = {0, };
	sabr_token_t value_token_b = {0, };
	sabr_token_t result_token = {0, };
	sabr_value_t value_a;
	sabr_value_t value_b;
	sabr_value_t result_value;

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_b = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_a = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_a, &value_a)) goto FREE_ALL;

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_b, &value_b)) goto FREE_ALL;

	result_value.i = (value_a.u > value_b.u) ? 1 : 0;

	result_token = t;
	result_token.is_generated = true;
	
	if (asprintf(&(result_token.data), "%" PRId64, result_value.i) == -1) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, result_token)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	result = true;
FREE_ALL:
	if (!result) {
		free(result_token.data);
	}
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_uleq(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_token_t value_token_a = {0, };
	sabr_token_t value_token_b = {0, };
	sabr_token_t result_token = {0, };
	sabr_value_t value_a;
	sabr_value_t value_b;
	sabr_value_t result_value;

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_b = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_a = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_a, &value_a)) goto FREE_ALL;

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_b, &value_b)) goto FREE_ALL;

	result_value.i = (value_a.u >= value_b.u) ? 1 : 0;

	result_token = t;
	result_token.is_generated = true;
	
	if (asprintf(&(result_token.data), "%" PRId64, result_value.i) == -1) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, result_token)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	result = true;
FREE_ALL:
	if (!result) {
		free(result_token.data);
	}
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_fadd(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_token_t value_token_a = {0, };
	sabr_token_t value_token_b = {0, };
	sabr_token_t result_token = {0, };
	sabr_value_t value_a;
	sabr_value_t value_b;
	sabr_value_t result_value;

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_b = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_a = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_a, &value_a)) goto FREE_ALL;

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_b, &value_b)) goto FREE_ALL;

	result_value.f = value_a.f + value_b.f;

	result_token = t;
	result_token.is_generated = true;
	
	if (asprintf(&(result_token.data), "%lf", result_value.f) == -1) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, result_token)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	result = true;
FREE_ALL:
	if (!result) {
		free(result_token.data);
	}
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_fsub(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_token_t value_token_a = {0, };
	sabr_token_t value_token_b = {0, };
	sabr_token_t result_token = {0, };
	sabr_value_t value_a;
	sabr_value_t value_b;
	sabr_value_t result_value;

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_b = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_a = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_a, &value_a)) goto FREE_ALL;

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_b, &value_b)) goto FREE_ALL;

	result_value.f = value_a.f - value_b.f;

	result_token = t;
	result_token.is_generated = true;
	
	if (asprintf(&(result_token.data), "%lf", result_value.f) == -1) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, result_token)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	result = true;
FREE_ALL:
	if (!result) {
		free(result_token.data);
	}
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_fmul(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_token_t value_token_a = {0, };
	sabr_token_t value_token_b = {0, };
	sabr_token_t result_token = {0, };
	sabr_value_t value_a;
	sabr_value_t value_b;
	sabr_value_t result_value;

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_b = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_a = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_a, &value_a)) goto FREE_ALL;

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_b, &value_b)) goto FREE_ALL;

	result_value.f = value_a.f * value_b.f;

	result_token = t;
	result_token.is_generated = true;
	
	if (asprintf(&(result_token.data), "%lf", result_value.f) == -1) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, result_token)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	result = true;
FREE_ALL:
	if (!result) {
		free(result_token.data);
	}
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_fdiv(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_token_t value_token_a = {0, };
	sabr_token_t value_token_b = {0, };
	sabr_token_t result_token = {0, };
	sabr_value_t value_a;
	sabr_value_t value_b;
	sabr_value_t result_value;

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_b = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_a = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_a, &value_a)) goto FREE_ALL;

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_b, &value_b)) goto FREE_ALL;

	result_value.f = value_a.f / value_b.f;

	result_token = t;
	result_token.is_generated = true;
	
	if (asprintf(&(result_token.data), "%lf", result_value.f) == -1) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, result_token)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	result = true;
FREE_ALL:
	if (!result) {
		free(result_token.data);
	}
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_fmod(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_token_t value_token_a = {0, };
	sabr_token_t value_token_b = {0, };
	sabr_token_t result_token = {0, };
	sabr_value_t value_a;
	sabr_value_t value_b;
	sabr_value_t result_value;

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_b = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_a = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_a, &value_a)) goto FREE_ALL;

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_b, &value_b)) goto FREE_ALL;

	result_value.f = fmod(value_a.f, value_b.f);

	result_token = t;
	result_token.is_generated = true;
	
	if (asprintf(&(result_token.data), "%lf", result_value.f) == -1) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, result_token)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	result = true;
FREE_ALL:
	if (!result) {
		free(result_token.data);
	}
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_fgrt(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_token_t value_token_a = {0, };
	sabr_token_t value_token_b = {0, };
	sabr_token_t result_token = {0, };
	sabr_value_t value_a;
	sabr_value_t value_b;
	sabr_value_t result_value;

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_b = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_a = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_a, &value_a)) goto FREE_ALL;

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_b, &value_b)) goto FREE_ALL;

	result_value.i = (value_a.f < value_b.f) ? 1 : 0;

	result_token = t;
	result_token.is_generated = true;
	
	if (asprintf(&(result_token.data), "%" PRId64, result_value.i) == -1) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, result_token)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	result = true;
FREE_ALL:
	if (!result) {
		free(result_token.data);
	}
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_fgeq(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_token_t value_token_a = {0, };
	sabr_token_t value_token_b = {0, };
	sabr_token_t result_token = {0, };
	sabr_value_t value_a;
	sabr_value_t value_b;
	sabr_value_t result_value;

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_b = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_a = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_a, &value_a)) goto FREE_ALL;

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_b, &value_b)) goto FREE_ALL;

	result_value.i = (value_a.f <= value_b.f) ? 1 : 0;

	result_token = t;
	result_token.is_generated = true;
	
	if (asprintf(&(result_token.data), "%" PRId64, result_value.i) == -1) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, result_token)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	result = true;
FREE_ALL:
	if (!result) {
		free(result_token.data);
	}
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_flst(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_token_t value_token_a = {0, };
	sabr_token_t value_token_b = {0, };
	sabr_token_t result_token = {0, };
	sabr_value_t value_a;
	sabr_value_t value_b;
	sabr_value_t result_value;

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_b = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_a = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_a, &value_a)) goto FREE_ALL;

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_b, &value_b)) goto FREE_ALL;

	result_value.i = (value_a.f > value_b.f) ? 1 : 0;

	result_token = t;
	result_token.is_generated = true;
	
	if (asprintf(&(result_token.data), "%" PRId64, result_value.i) == -1) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, result_token)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	result = true;
FREE_ALL:
	if (!result) {
		free(result_token.data);
	}
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_fleq(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_token_t value_token_a = {0, };
	sabr_token_t value_token_b = {0, };
	sabr_token_t result_token = {0, };
	sabr_value_t value_a;
	sabr_value_t value_b;
	sabr_value_t result_value;

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_b = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_a = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_a, &value_a)) goto FREE_ALL;

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_b, &value_b)) goto FREE_ALL;

	result_value.i = (value_a.f >= value_b.f) ? 1 : 0;

	result_token = t;
	result_token.is_generated = true;
	
	if (asprintf(&(result_token.data), "%" PRId64, result_value.i) == -1) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, result_token)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	result = true;
FREE_ALL:
	if (!result) {
		free(result_token.data);
	}
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_and(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_token_t value_token_a = {0, };
	sabr_token_t value_token_b = {0, };
	sabr_token_t result_token = {0, };
	sabr_value_t value_a;
	sabr_value_t value_b;
	sabr_value_t result_value;

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_b = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_a = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_a, &value_a)) goto FREE_ALL;

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_b, &value_b)) goto FREE_ALL;

	result_value.i = value_a.i & value_b.i;

	result_token = t;
	result_token.is_generated = true;
	
	if (asprintf(&(result_token.data), "%" PRId64, result_value.i) == -1) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, result_token)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	result = true;
FREE_ALL:
	if (!result) {
		free(result_token.data);
	}
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_or(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_token_t value_token_a = {0, };
	sabr_token_t value_token_b = {0, };
	sabr_token_t result_token = {0, };
	sabr_value_t value_a;
	sabr_value_t value_b;
	sabr_value_t result_value;

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_b = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_a = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_a, &value_a)) goto FREE_ALL;

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_b, &value_b)) goto FREE_ALL;

	result_value.i = value_a.i | value_b.i;

	result_token = t;
	result_token.is_generated = true;
	
	if (asprintf(&(result_token.data), "%" PRId64, result_value.i) == -1) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, result_token)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	result = true;
FREE_ALL:
	if (!result) {
		free(result_token.data);
	}
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_xor(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_token_t value_token_a = {0, };
	sabr_token_t value_token_b = {0, };
	sabr_token_t result_token = {0, };
	sabr_value_t value_a;
	sabr_value_t value_b;
	sabr_value_t result_value;

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_b = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_a = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_a, &value_a)) goto FREE_ALL;

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_b, &value_b)) goto FREE_ALL;

	result_value.i = value_a.i ^ value_b.i;

	result_token = t;
	result_token.is_generated = true;
	
	if (asprintf(&(result_token.data), "%" PRId64, result_value.i) == -1) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, result_token)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	result = true;
FREE_ALL:
	if (!result) {
		free(result_token.data);
	}
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_not(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_token_t value_token_a = {0, };
	sabr_token_t result_token = {0, };
	sabr_value_t value_a;
	sabr_value_t result_value;

	bool result = false;

	if (output_tokens->size < 1) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_a = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_a, &value_a)) goto FREE_ALL;

	result_value.i = ~value_a.i;

	result_token = t;
	result_token.is_generated = true;
	
	if (asprintf(&(result_token.data), "%" PRId64, result_value.i) == -1) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, result_token)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	result = true;
FREE_ALL:
	if (!result) {
		free(result_token.data);
	}
	free(value_token_a.data);
	return result;
}

const bool sabr_compiler_preproc_lsft(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_token_t value_token_a = {0, };
	sabr_token_t value_token_b = {0, };
	sabr_token_t result_token = {0, };
	sabr_value_t value_a;
	sabr_value_t value_b;
	sabr_value_t result_value;

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_b = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_a = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_a, &value_a)) goto FREE_ALL;

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_b, &value_b)) goto FREE_ALL;

	result_value.u = value_a.u << value_b.u;

	result_token = t;
	result_token.is_generated = true;
	
	if (asprintf(&(result_token.data), "%" PRId64, result_value.u) == -1) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, result_token)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	result = true;
FREE_ALL:
	if (!result) {
		free(result_token.data);
	}
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_rsft(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_token_t value_token_a = {0, };
	sabr_token_t value_token_b = {0, };
	sabr_token_t result_token = {0, };
	sabr_value_t value_a;
	sabr_value_t value_b;
	sabr_value_t result_value;

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_b = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_a = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_a, &value_a)) goto FREE_ALL;

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_b, &value_b)) goto FREE_ALL;

	result_value.u = value_a.u >> value_b.u;

	result_token = t;
	result_token.is_generated = true;
	
	if (asprintf(&(result_token.data), "%" PRId64, result_value.u) == -1) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, result_token)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	result = true;
FREE_ALL:
	if (!result) {
		free(result_token.data);
	}
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_ftoi(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_token_t value_token_a = {0, };
	sabr_token_t result_token = {0, };
	sabr_value_t value_a;
	sabr_value_t result_value;

	bool result = false;

	if (output_tokens->size < 1) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_a = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_a, &value_a)) goto FREE_ALL;

	result_value.i = (int64_t) value_a.f;

	result_token = t;
	result_token.is_generated = true;
	
	if (asprintf(&(result_token.data), "%" PRId64, result_value.i) == -1) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, result_token)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	result = true;
FREE_ALL:
	if (!result) {
		free(result_token.data);
	}
	free(value_token_a.data);
	return result;
}

const bool sabr_compiler_preproc_itof(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_token_t value_token_a = {0, };
	sabr_token_t result_token = {0, };
	sabr_value_t value_a;
	sabr_value_t result_value;

	bool result = false;

	if (output_tokens->size < 1) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_a = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_a, &value_a)) goto FREE_ALL;

	result_value.f = (double) value_a.i;

	result_token = t;
	result_token.is_generated = true;
	
	if (asprintf(&(result_token.data), "%lf", result_value.f) == -1) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, result_token)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	result = true;
FREE_ALL:
	if (!result) {
		free(result_token.data);
	}
	free(value_token_a.data);
	return result;
}

const bool sabr_compiler_preproc_fmti(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_token_t value_token_a = {0, };
	sabr_token_t result_token = {0, };
	sabr_value_t value_a;
	sabr_value_t result_value;

	bool result = false;

	if (output_tokens->size < 1) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_a = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_a, &value_a)) goto FREE_ALL;

	result_value.i = value_a.i;

	result_token = t;
	result_token.is_generated = true;
	
	if (asprintf(&(result_token.data), "%" PRId64, result_value.i) == -1) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, result_token)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	result = true;
FREE_ALL:
	if (!result) {
		free(result_token.data);
	}
	free(value_token_a.data);
	return result;
}

const bool sabr_compiler_preproc_fmtu(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_token_t value_token_a = {0, };
	sabr_token_t result_token = {0, };
	sabr_value_t value_a;
	sabr_value_t result_value;

	bool result = false;

	if (output_tokens->size < 1) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_a = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_a, &value_a)) goto FREE_ALL;

	result_value.u = value_a.u;

	result_token = t;
	result_token.is_generated = true;
	
	if (asprintf(&(result_token.data), "%" PRIu64, result_value.u) == -1) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, result_token)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	result = true;
FREE_ALL:
	if (!result) {
		free(result_token.data);
	}
	free(value_token_a.data);
	return result;
}

const bool sabr_compiler_preproc_fmtf(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_token_t value_token_a = {0, };
	sabr_token_t result_token = {0, };
	sabr_value_t value_a;
	sabr_value_t result_value;

	bool result = false;

	if (output_tokens->size < 1) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_a = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_a, &value_a)) goto FREE_ALL;

	result_value.f = value_a.f;

	result_token = t;
	result_token.is_generated = true;
	
	if (asprintf(&(result_token.data), "%lf", result_value.f) == -1) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	if (!vector_push_back(sabr_token_t, output_tokens, result_token)) {
		fputs(sabr_errmsg_alloc, stderr); goto FREE_ALL;
	}

	result = true;
FREE_ALL:
	if (!result) {
		free(result_token.data);
	}
	free(value_token_a.data);
	return result;
}

const bool sabr_compiler_preproc_echo(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_token_t value_token_a = {0, };

	bool result = false;

	if (output_tokens->size < 1) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_a = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	printf("%s", value_token_a.data);

	result = true;
FREE_ALL:
	free(value_token_a.data);
	return result;
}

const bool sabr_compiler_preproc_emit(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	sabr_token_t value_token_a = {0, };
	sabr_value_t value_a;

	bool result = false;

	if (output_tokens->size < 1) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}
	
	value_token_a = *vector_back(sabr_token_t, output_tokens);
	if (!vector_pop_back(sabr_token_t, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr); goto FREE_ALL;
	}

	if (!sabr_compiler_preprocess_parse_value(comp, value_token_a, &value_a)) goto FREE_ALL;

	if (value_a.u < 127) {
		putchar(value_a.u);
	}
	else {
		char out[8];
		size_t rc = c32rtomb(out, (char32_t) value_a.u, &(comp->convert_state));
		if (rc == -1) {
			goto FREE_ALL;
		}
		out[rc] = 0;
		fputs(out, stdout);
	}

	result = true;
FREE_ALL:
	free(value_token_a.data);
	return result;
}

const bool sabr_compiler_preproc_show(sabr_compiler_t* comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) {
	printf("%zu - [ ", output_tokens->size);
	for (size_t i = 0; i < output_tokens->size; i++) {
		sabr_token_t t = *vector_at(sabr_token_t, output_tokens, i);
		printf("%s ", t.data);
	}
	fputs("]\n", stdout);

	return true;
}


const bool (*preproc_keyword_functions[])(sabr_compiler_t* const comp, sabr_word_t w, sabr_token_t t, vector(sabr_token_t)* output_tokens) = {
	sabr_compiler_preproc_func,
	sabr_compiler_preproc_macro,
	sabr_compiler_preproc_isdef,
	sabr_compiler_preproc_undef,
	sabr_compiler_preproc_getdef,
	sabr_compiler_preproc_lfunc,
	sabr_compiler_preproc_lmacro,
	sabr_compiler_preproc_lisdef,
	sabr_compiler_preproc_lundef,
	sabr_compiler_preproc_lgetdef,
	sabr_compiler_preproc_import,
	sabr_compiler_preproc_eval,
	sabr_compiler_preproc_if,
	sabr_compiler_preproc_while,
	sabr_compiler_preproc_break,
	sabr_compiler_preproc_continue,
	sabr_compiler_preproc_concat,
	sabr_compiler_preproc_substr,
	sabr_compiler_preproc_trim,
	sabr_compiler_preproc_compare,
	sabr_compiler_preproc_len,
	sabr_compiler_preproc_drop,
	sabr_compiler_preproc_nip,
	sabr_compiler_preproc_dup,
	sabr_compiler_preproc_over,
	sabr_compiler_preproc_tuck,
	sabr_compiler_preproc_swap,
	sabr_compiler_preproc_rot,
	sabr_compiler_preproc_2drop,
	sabr_compiler_preproc_2nip,
	sabr_compiler_preproc_2dup,
	sabr_compiler_preproc_2over,
	sabr_compiler_preproc_2tuck,
	sabr_compiler_preproc_2swap,
	sabr_compiler_preproc_2rot,
	sabr_compiler_preproc_add,
	sabr_compiler_preproc_sub,
	sabr_compiler_preproc_mul,
	sabr_compiler_preproc_div,
	sabr_compiler_preproc_mod,
	sabr_compiler_preproc_udiv,
	sabr_compiler_preproc_umod,
	sabr_compiler_preproc_equ,
	sabr_compiler_preproc_neq,
	sabr_compiler_preproc_grt,
	sabr_compiler_preproc_geq,
	sabr_compiler_preproc_lst,
	sabr_compiler_preproc_leq,
	sabr_compiler_preproc_ugrt,
	sabr_compiler_preproc_ugeq,
	sabr_compiler_preproc_ulst,
	sabr_compiler_preproc_uleq,
	sabr_compiler_preproc_fadd,
	sabr_compiler_preproc_fsub,
	sabr_compiler_preproc_fmul,
	sabr_compiler_preproc_fdiv,
	sabr_compiler_preproc_fmod,
	sabr_compiler_preproc_fgrt,
	sabr_compiler_preproc_fgeq,
	sabr_compiler_preproc_flst,
	sabr_compiler_preproc_fleq,
	sabr_compiler_preproc_and,
	sabr_compiler_preproc_or,
	sabr_compiler_preproc_xor,
	sabr_compiler_preproc_not,
	sabr_compiler_preproc_lsft,
	sabr_compiler_preproc_rsft,
	sabr_compiler_preproc_ftoi,
	sabr_compiler_preproc_itof,
	sabr_compiler_preproc_fmti,
	sabr_compiler_preproc_fmtu,
	sabr_compiler_preproc_fmtf,
	sabr_compiler_preproc_echo,
	sabr_compiler_preproc_emit,
	sabr_compiler_preproc_show
};