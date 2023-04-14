#ifndef __PREPROC_OPERATION_H__
#define __PREPROC_OPERATION_H__

#include "compiler.h"

extern const bool (*preproc_keyword_functions[])(sabr_compiler* const comp, word w, token t, vector(token)* output_tokens);

const bool sabr_compiler_preproc_define(sabr_compiler* comp, word w, token t, vector(token)* output_tokens);

#endif