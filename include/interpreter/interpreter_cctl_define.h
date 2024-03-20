#ifndef __INTERPRETER_CCTL_DEFINE_H__
#define __INTERPRETER_CCTL_DEFINE_H__

#include "cctl/deque.h"
#include "cctl/rbt.h"
#include "cctl/vector.h"

#include "value.h"

#include "interpreter_data.h"

rbt_fd(sabr_def_data_t);
cctl_ptr_def(rbt(sabr_def_data_t));
deque_fd(cctl_ptr(rbt(sabr_def_data_t)));

deque_fd(sabr_value_t);
deque_fd(size_t);
deque_fd(sabr_for_data_t);
deque_fd(sabr_cs_data_t);

vector_fd(sabr_value_t);
cctl_ptr_def(vector(sabr_value_t));
vector_fd(cctl_ptr(vector(sabr_value_t)));

rbt_imp_h(sabr_def_data_t);
deque_imp_h(cctl_ptr(rbt(sabr_def_data_t)));
deque_imp_h(sabr_value_t);
deque_imp_h(size_t);
deque_imp_h(sabr_for_data_t);
deque_imp_h(sabr_cs_data_t);

vector_imp_h(cctl_ptr(vector(sabr_value_t)));

#endif