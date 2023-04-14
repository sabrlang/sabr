#include "preproc_operation.h"

const bool sabr_compiler_preproc_define(sabr_compiler* comp, word w, token t, vector(token)* output_tokens) {
	token code_token = {0, };
	token identifier_token = {0, };

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
		fputs(sabr_invalid_identifier_format, stderr);
		goto FREE_ALL;
	}

	word macro_word;
	macro_word.type = WT_PREPROC_IDFR;
	macro_word.data.macro_code = code_token;

	if (!trie_insert(word, &comp->preproc_dictionary, identifier_token.data + 1, macro_word)) {
		fputs(sabr_errmsg_alloc, stderr);
		goto FREE_ALL;
	}

	free(identifier_token.data);

	return true;

FREE_ALL:
	free(code_token.data);
	free(identifier_token.data);
	return false;
}

const bool (*preproc_keyword_functions[])(sabr_compiler* const comp, word w, token t, vector(token)* output_tokens) = {
	sabr_compiler_preproc_define
};