#ifndef __INTERPRETER_SABR_OP_H__
#define __INTERPRETER_SABR_OP_H__

#include "interpreter.h"

typedef struct sabr_interpreter_struct sabr_interpreter_t;

extern size_t sabr_interpreter_op_functions_len;
extern const uint32_t (*sabr_interpreter_op_functions[])(sabr_interpreter_t*, sabr_bcop_t, size_t*);

#define sabr_interpreter_op(OP) cctl_join(sabr_interpreter, OP)

const uint32_t sabr_interpreter_op(op_exit)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_value)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_if)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_jump)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_for)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_for_from)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_for_to)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_for_step)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_for_check)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_for_next)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_for_end)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_switch)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_switch_case)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_switch_end)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_lambda)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_return)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_local)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_local_end)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_define)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_struct)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_member)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_struct_end)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_struct_exec)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_set)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_exec)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_addr)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_ref)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_call_bif)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_add)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_sub)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_mul)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_div)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_mod)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_udiv)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_umod)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_neg)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_inc)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_dec)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_equ)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_neq)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_grt)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_geq)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_lst)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_leq)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_ugrt)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_ugeq)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_ulst)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_uleq)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_fadd)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_fsub)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_fmul)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_fdiv)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_fmod)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_fneg)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_fequ)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_fneq)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_fgrt)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_fgeq)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_flst)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_fleq)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_band)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_bor)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_bxor)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_bnot)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_blsft)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_brsft)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_drop)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_nip)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_dup)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_over)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_tuck)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_swap)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_rot)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_tdrop)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_tnip)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_tdup)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_tover)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_ttuck)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_tswap)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_trot)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_alloc)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_resize)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_free)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_allot)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_fetch)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_store)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_array)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_array_comma)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_array_end)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_itof)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_utof)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_ftoi)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_ftou)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_getc)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_geti)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_getu)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_getf)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_gets)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_putc)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_puti)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_putu)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_putf)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_puts)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);
const uint32_t sabr_interpreter_op(op_show)(sabr_interpreter_t* inter, sabr_bcop_t bcop, size_t* index);

#endif
