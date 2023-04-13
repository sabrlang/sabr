#include "utils.h"

char* new_string_slice(const char* source, size_t begin_index, size_t end_index) {
	size_t size = end_index - begin_index + 1;

	char* new_string = (char*) malloc(size * sizeof(char));
	if (!new_string) return NULL;

	strncpy(new_string, source + begin_index, size - 1);
	new_string[size - 1] = '\0';

	return new_string;
}