#include "compiler_utils.h"

char* sabr_new_string_slice(const char* source, size_t begin_index, size_t end_index) {
	size_t size = end_index - begin_index + 1;

	char* new_string = (char*) malloc(size * sizeof(char));
	if (!new_string) return NULL;

	strncpy(new_string, source + begin_index, size - 1);
	new_string[size - 1] = '\0';

	return new_string;
}

char* sabr_new_string_copy(const char* source) {
	return sabr_new_string_slice(source, 0, strlen(source));
}

char* sabr_new_string_append(const char* dest, const char* origin) {
	char* new_string = NULL;
	asprintf(&new_string, "%s%s", dest, origin);
	return new_string;
}

void sabr_free_token_vector(vector(token)* tokens) {
	if (tokens) {
		for (size_t i = 0; i < tokens->size; i++) {
			token t = *vector_at(token, tokens, i);
			free(t.data);
		}
		vector_free(token, tokens);
	}
}

void sabr_free_word_trie(trie(word)* dictionary) {
	if (!dictionary) return;

	if (dictionary->existence) {
		word w = dictionary->data;
		switch (w.type) {
			case WT_PREPROC_IDFR: {
				token t = w.data.def_data.def_code;
				free(t.data);
			} break;
			default:
				break;
		}
	}

	for (size_t i = 0; i < 256; i++) {
		sabr_free_word_trie(dictionary->children[i]);
		free(dictionary->children[i]);
	}
}
