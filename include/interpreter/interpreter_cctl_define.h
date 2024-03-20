#ifndef __INTERPRETER_CCTL_DEFINE_H__
#define __INTERPRETER_CCTL_DEFINE_H__

#include "cctl/deque.h"
#include "cctl/rbt.h"
#include "cctl/vector.h"

#include "value.h"

#include "interpreter_data.h"

rbt_fd(def_data);
cctl_ptr_def(rbt(def_data));
deque_fd(cctl_ptr(rbt(def_data)));

deque_fd(value);
deque_fd(size_t);
deque_fd(for_data);
deque_fd(cs_data);

vector_fd(value);
cctl_ptr_def(vector(value));
vector_fd(cctl_ptr(vector(value)));

rbt_imp_h(def_data);
deque_imp_h(cctl_ptr(rbt(def_data)));
deque_imp_h(value);
deque_imp_h(size_t);
deque_imp_h(for_data);
deque_imp_h(cs_data);

vector_imp_h(cctl_ptr(vector(value)));

#endif