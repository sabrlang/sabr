#ifndef __BIF_DEF_H__
#define __BIF_DEF_H__

#include "interpreter.h"

typedef struct sabr_interpreter_struct sabr_interpreter_t;

typedef const uint32_t (*sabr_bif_func_t)(sabr_interpreter_t*);

#define sabr_bif_func(MODULE, FUNC) cctl_join(sabr_bif, cctl_join(MODULE, FUNC))

#endif