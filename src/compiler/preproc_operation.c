#include "preproc_operation.h"

const bool sabr_compiler_preproc_define(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	token code_token = {0, };
	token identifier_token = {0, };

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}

	code_token = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}

	identifier_token = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}

	if (identifier_token.data[0] != '$') {
		fputs(sabr_errmsg_invalid_ident_fmt, stderr);
		goto FREE_ALL;
	}

	token macro_code_token = code_token;
	macro_code_token.data = sabr_new_string_copy(code_token.data);

	word macro_word;
	macro_word.type = WT_PREPROC_IDFR;
	macro_word.data.macro_code = macro_code_token;

	if (!trie_insert(word, &comp->preproc_dictionary, identifier_token.data + 1, macro_word)) {
		fputs(sabr_errmsg_alloc, stderr);
		goto FREE_ALL;
	}

	result = !result;
FREE_ALL:
	free(code_token.data);
	free(identifier_token.data);
	return result;
}

const bool sabr_compiler_preproc_defined(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	token identifier_token = {0, };
	token result_token = {0, };

	bool result = false;

	if (output_tokens->size < 1) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}

	identifier_token = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}

	if (identifier_token.data[0] != '$') {
		fputs(sabr_errmsg_invalid_ident_fmt, stderr);
		goto FREE_ALL;
	}
	
	word* identifier_word = trie_find(word, &comp->preproc_dictionary, identifier_token.data + 1);
	int flag = identifier_word ? ((identifier_word->type == WT_PREPROC_IDFR) ? 1 : 0) : 0;

	if (asprintf(&(result_token.data), "%d", flag) == -1) {
		fputs(sabr_errmsg_alloc, stderr);
		goto FREE_ALL;
	}

	result_token.begin_pos = t.begin_pos;
	result_token.end_pos = t.end_pos;
	result_token.begin_index = t.begin_index;
	result_token.end_index = t.end_index;
	result_token.textcode_index = t.textcode_index;
	result_token.is_generated = true;

	if (!vector_push_back(token, output_tokens, result_token)) {
		fputs(sabr_errmsg_alloc, stderr);
		goto FREE_ALL;
	}

	result = !result;
FREE_ALL:
	if (!result) {
		free(result_token.data);
	}
	free(identifier_token.data);
	return result;
}

const bool sabr_compiler_preproc_undef(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	token identifier_token = {0, };

	bool result = false;

	if (output_tokens->size < 1) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}

	identifier_token = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}

	if (identifier_token.data[0] != '$') {
		fputs(sabr_errmsg_invalid_ident_fmt, stderr);
		goto FREE_ALL;
	}

	trie_remove(word, &comp->preproc_dictionary, identifier_token.data + 1);

	result = !result;
FREE_ALL:
	free(identifier_token.data);
	return result;
}

const bool sabr_compiler_preproc_import(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	token filename_token = {0, };
	char filename_full[PATH_MAX] = {0, };
	size_t textcode_index;
	vector(token)* preprocessed_tokens = NULL;

	bool result = false;

	if (output_tokens->size < 1) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	filename_token = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}

#if defined(_WIN32)
	if (!_fullpath(filename_full, filename_token.data, PATH_MAX)) {
		fputs(sabr_errmsg_fullpath, stderr);
		goto FREE_ALL;
	}
#else
	if (!(realpath(filename_token.data, filename_full))) goto FAILURE_FILEPATH;
#endif

	if (!trie_find(size_t, &comp->filename_trie, filename_full)) {
		if (!sabr_compiler_load_file(comp, filename_full, &textcode_index)) {
			goto FREE_ALL;
		}

		preprocessed_tokens = sabr_compiler_preprocess_textcode(comp, textcode_index);
		if (!preprocessed_tokens) {
			goto FREE_ALL;
		}

		for (size_t i = 0; i < preprocessed_tokens->size; i++) {
			if (!vector_push_back(token, output_tokens, *vector_at(token, preprocessed_tokens, i))) {
				fputs(sabr_errmsg_alloc, stderr);
				goto FREE_ALL;
			}
		}
	}

	result = !result;
FREE_ALL:
	if (!result) {
		sabr_free_token_vector(preprocessed_tokens);
	}
	free(preprocessed_tokens);
	free(filename_token.data);
	return result;
}

const bool sabr_compiler_preproc_include(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	token filename_token = {0, };

	bool result = false;

	if (output_tokens->size < 1) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	filename_token = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}

	result = !result;
FREE_ALL:
	free(filename_token.data);
	return result;
}

const bool sabr_compiler_preproc_eval(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	token code_token = {0, };

	bool result = false;

	if (output_tokens->size < 1) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	code_token = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}

	result = !result;
FREE_ALL:
	free(code_token.data);
	return result;
}

const bool sabr_compiler_preproc_if(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	token code_token_a = {0, };
	token code_token_b = {0, };
	token flag_token = {0, };

	bool result = false;

	if (output_tokens->size < 3) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	code_token_b = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	code_token_a = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	flag_token = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}

	result = !result;
FREE_ALL:
	free(code_token_a.data);
	free(code_token_b.data);
	free(flag_token.data);
	return result;
}

const bool sabr_compiler_preproc_concat(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	token text_token_a = {0, };
	token text_token_b = {0, };

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	text_token_b = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	text_token_a = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}

	result = !result;
FREE_ALL:
	free(text_token_a.data);
	free(text_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_substr(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	token text_token = {0, };
	token begin_token = {0, };
	token end_token = {0, };

	bool result = false;

	if (output_tokens->size < 3) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	end_token = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	begin_token = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	text_token = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}

	result = !result;
FREE_ALL:
	free(text_token.data);
	free(begin_token.data);
	free(end_token.data);
	return result;
}

const bool sabr_compiler_preproc_compare(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	token text_token_a = {0, };
	token text_token_b = {0, };

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	text_token_b = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	text_token_a = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}

	result = !result;
FREE_ALL:
	free(text_token_a.data);
	free(text_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_len(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	token text_token = {0, };

	bool result = false;

	if (output_tokens->size < 1) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	text_token = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}

	result = !result;
FREE_ALL:
	free(text_token.data);
	return result;
}

const bool sabr_compiler_preproc_drop(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	token value_token_a = {0, };

	bool result = false;

	if (output_tokens->size < 1) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_a = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}

	result = !result;
FREE_ALL:
	free(value_token_a.data);
	return result;
}

const bool sabr_compiler_preproc_nip(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	token value_token_a = {0, };
	token value_token_b = {0, };

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_b = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_a = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}

	result = !result;
FREE_ALL:
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_dup(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	token value_token_a = {0, };

	bool result = false;

	if (output_tokens->size < 1) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_a = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}

	result = !result;
FREE_ALL:
	free(value_token_a.data);
	return result;
}

const bool sabr_compiler_preproc_over(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	token value_token_a = {0, };
	token value_token_b = {0, };

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_b = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_a = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}

	result = !result;
FREE_ALL:
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_tuck(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	token value_token_a = {0, };
	token value_token_b = {0, };

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_b = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_a = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}

	result = !result;
FREE_ALL:
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_swap(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	token value_token_a = {0, };
	token value_token_b = {0, };

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_b = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_a = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}

	result = !result;
FREE_ALL:
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_rot(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	token value_token_a = {0, };
	token value_token_b = {0, };
	token value_token_c = {0, };

	bool result = false;

	if (output_tokens->size < 3) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_c = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_b = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_a = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}

	result = !result;
FREE_ALL:
	free(value_token_a.data);
	free(value_token_b.data);
	free(value_token_c.data);
	return result;
}

const bool sabr_compiler_preproc_2drop(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	token value_token_a = {0, };
	token value_token_b = {0, };

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_b = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_a = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}

	result = !result;
FREE_ALL:
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_2nip(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	token value_token_a = {0, };
	token value_token_b = {0, };
	token value_token_c = {0, };
	token value_token_d = {0, };

	bool result = false;

	if (output_tokens->size < 4) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_d = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_c = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_b = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_a = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}

	result = !result;
FREE_ALL:
	free(value_token_a.data);
	free(value_token_b.data);
	free(value_token_c.data);
	free(value_token_d.data);
	return result;
}

const bool sabr_compiler_preproc_2dup(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	token value_token_a = {0, };
	token value_token_b = {0, };

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_b = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_a = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}

	result = !result;
FREE_ALL:
	free(value_token_a.data);
	free(value_token_b.data);

	return result;
}

const bool sabr_compiler_preproc_2over(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	token value_token_a = {0, };
	token value_token_b = {0, };
	token value_token_c = {0, };
	token value_token_d = {0, };

	bool result = false;

	if (output_tokens->size < 4) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_d = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_c = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_b = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_a = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}

	result = !result;
FREE_ALL:
	free(value_token_a.data);
	free(value_token_b.data);
	free(value_token_c.data);
	free(value_token_d.data);
	return result;
}

const bool sabr_compiler_preproc_2tuck(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	token value_token_a = {0, };
	token value_token_b = {0, };
	token value_token_c = {0, };
	token value_token_d = {0, };

	bool result = false;

	if (output_tokens->size < 4) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_d = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_c = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_b = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_a = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}

	result = !result;
FREE_ALL:
	free(value_token_a.data);
	free(value_token_b.data);
	free(value_token_c.data);
	free(value_token_d.data);
	return result;
}

const bool sabr_compiler_preproc_2swap(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	token value_token_a = {0, };
	token value_token_b = {0, };
	token value_token_c = {0, };
	token value_token_d = {0, };

	bool result = false;

	if (output_tokens->size < 4) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_d = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_c = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_b = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_a = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}

	result = !result;
FREE_ALL:
	free(value_token_a.data);
	free(value_token_b.data);
	free(value_token_c.data);
	free(value_token_d.data);
	return result;
}

const bool sabr_compiler_preproc_2rot(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	token value_token_a = {0, };
	token value_token_b = {0, };
	token value_token_c = {0, };
	token value_token_d = {0, };
	token value_token_e = {0, };
	token value_token_f = {0, };

	bool result = false;

	if (output_tokens->size < 6) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_f = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_e = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_d = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_c = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_b = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_a = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}

	result = !result;
FREE_ALL:
	free(value_token_a.data);
	free(value_token_b.data);
	free(value_token_c.data);
	free(value_token_d.data);
	free(value_token_e.data);
	free(value_token_f.data);
	return result;
}

const bool sabr_compiler_preproc_add(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	token value_token_a = {0, };
	token value_token_b = {0, };

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}

	result = !result;
FREE_ALL:
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_sub(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	token value_token_a = {0, };
	token value_token_b = {0, };

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_b = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_a = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}

	result = !result;
FREE_ALL:
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_mul(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	token value_token_a = {0, };
	token value_token_b = {0, };

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_b = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_a = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}

	result = !result;
FREE_ALL:
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_div(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	token value_token_a = {0, };
	token value_token_b = {0, };

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_b = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_a = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}

	result = !result;
FREE_ALL:
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_mod(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	token value_token_a = {0, };
	token value_token_b = {0, };

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_b = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_a = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}

	result = !result;
FREE_ALL:
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_udiv(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	token value_token_a = {0, };
	token value_token_b = {0, };

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_b = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_a = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}

	result = !result;
FREE_ALL:
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_umod(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	token value_token_a = {0, };
	token value_token_b = {0, };

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_b = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_a = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}

	result = !result;
FREE_ALL:
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_equ(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	token value_token_a = {0, };
	token value_token_b = {0, };

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_b = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_a = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}

	result = !result;
FREE_ALL:
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_neq(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	token value_token_a = {0, };
	token value_token_b = {0, };

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_b = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_a = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}

	result = !result;
FREE_ALL:
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_grt(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	token value_token_a = {0, };
	token value_token_b = {0, };

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_b = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_a = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}

	result = !result;
FREE_ALL:
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_geq(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	token value_token_a = {0, };
	token value_token_b = {0, };

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_b = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_a = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}

	result = !result;
FREE_ALL:
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_lst(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	token value_token_a = {0, };
	token value_token_b = {0, };

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_b = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_a = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}

	result = !result;
FREE_ALL:
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_leq(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	token value_token_a = {0, };
	token value_token_b = {0, };

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_b = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_a = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}

	result = !result;
FREE_ALL:
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_fadd(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	token value_token_a = {0, };
	token value_token_b = {0, };

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_b = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_a = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}

	result = !result;
FREE_ALL:
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_fsub(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	token value_token_a = {0, };
	token value_token_b = {0, };

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_b = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_a = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}

	result = !result;
FREE_ALL:
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_fmul(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	token value_token_a = {0, };
	token value_token_b = {0, };

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_b = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_a = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}

	result = !result;
FREE_ALL:
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_fdiv(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	token value_token_a = {0, };
	token value_token_b = {0, };

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_b = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_a = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}

	result = !result;
FREE_ALL:
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_fmod(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	token value_token_a = {0, };
	token value_token_b = {0, };

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_b = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_a = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}

	result = !result;
FREE_ALL:
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_fgrt(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	token value_token_a = {0, };
	token value_token_b = {0, };

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_b = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_a = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}

	result = !result;
FREE_ALL:
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_fgeq(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	token value_token_a = {0, };
	token value_token_b = {0, };

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_b = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_a = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}

	result = !result;
FREE_ALL:
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_flst(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	token value_token_a = {0, };
	token value_token_b = {0, };

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_b = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_a = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}

	result = !result;
FREE_ALL:
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_fleq(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	token value_token_a = {0, };
	token value_token_b = {0, };

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_b = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_a = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}

	result = !result;
FREE_ALL:
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_and(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	token value_token_a = {0, };
	token value_token_b = {0, };

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_b = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_a = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}

	result = !result;
FREE_ALL:
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_or(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	token value_token_a = {0, };
	token value_token_b = {0, };

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_b = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_a = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}

	result = !result;
FREE_ALL:
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_xor(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	token value_token_a = {0, };
	token value_token_b = {0, };

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_b = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_a = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}

	result = !result;
FREE_ALL:
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_not(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	token value_token_a = {0, };

	bool result = false;

	if (output_tokens->size < 1) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_a = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}

	result = !result;
FREE_ALL:
	free(value_token_a.data);
	return result;
}

const bool sabr_compiler_preproc_lsft(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	token value_token_a = {0, };
	token value_token_b = {0, };

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_b = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_a = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}

	result = !result;
FREE_ALL:
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_rsft(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	token value_token_a = {0, };
	token value_token_b = {0, };

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_b = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_a = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}

	result = !result;
FREE_ALL:
	free(value_token_a.data);
	free(value_token_b.data);
	return result;
}

const bool sabr_compiler_preproc_ftoi(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	token value_token_a = {0, };

	bool result = false;

	if (output_tokens->size < 2) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_a = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}

	result = !result;
FREE_ALL:
	free(value_token_a.data);
	return result;
}

const bool sabr_compiler_preproc_itof(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	token value_token_a = {0, };

	bool result = false;

	if (output_tokens->size < 1) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_a = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}

	result = !result;
FREE_ALL:
	free(value_token_a.data);
	return result;
}

const bool sabr_compiler_preproc_fmti(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	token value_token_a = {0, };

	bool result = false;

	if (output_tokens->size < 1) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_a = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}

	result = !result;
FREE_ALL:
	free(value_token_a.data);
	return result;
}

const bool sabr_compiler_preproc_fmtu(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	token value_token_a = {0, };

	bool result = false;

	if (output_tokens->size < 1) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_a = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}

	result = !result;
FREE_ALL:
	free(value_token_a.data);
	return result;
}

const bool sabr_compiler_preproc_fmtf(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	token value_token_a = {0, };

	bool result = false;

	if (output_tokens->size < 1) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}
	
	value_token_a = *vector_back(token, output_tokens);
	if (!vector_pop_back(token, output_tokens)) {
		fputs(sabr_errmsg_stackunderflow, stderr);
		goto FREE_ALL;
	}

	result = !result;
FREE_ALL:
	free(value_token_a.data);
	return result;
}

const bool (*preproc_keyword_functions[])(sabr_compiler* const comp, word w, token t, vector(token)* output_tokens) = {
	sabr_compiler_preproc_define,
	sabr_compiler_preproc_defined,
	sabr_compiler_preproc_undef,
	sabr_compiler_preproc_import,
	sabr_compiler_preproc_include,
	sabr_compiler_preproc_eval,
	sabr_compiler_preproc_if,
	sabr_compiler_preproc_concat,
	sabr_compiler_preproc_substr,
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
	sabr_compiler_preproc_fmtf
};