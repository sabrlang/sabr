#ifndef __CCTL_DEFINE_H__
#define __CCTL_DEFINE_H__

#include "cctl/vector.h"

#include "value.h"
#include "bcop.h"

vector_fd(value);
vector_imp_h(value);

vector_fd(bytecode_operation);
vector_imp_h(bytecode_operation);

#endif