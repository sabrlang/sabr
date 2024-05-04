#ifndef __BIF_MATH_H__
#define __BIF_MATH_H__

#include "bif_def.h"

extern size_t sabr_bif_math_functions_len;
extern const uint32_t (*sabr_bif_math_functions[])(sabr_interpreter_t*);

const uint32_t sabr_bif_func(math, sin)(sabr_interpreter_t* inter);
const uint32_t sabr_bif_func(math, cos)(sabr_interpreter_t* inter);
const uint32_t sabr_bif_func(math, tan)(sabr_interpreter_t* inter);

#endif