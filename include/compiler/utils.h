#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* new_string_slice(const char* source, size_t begin_index, size_t end_index);

#endif