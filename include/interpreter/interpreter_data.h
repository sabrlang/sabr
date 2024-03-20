#ifndef __INTERPRETER_DATA_H__
#define __INTERPRETER_DATA_H__

#include <stdbool.h>

#include "value.h"

typedef enum for_type_enum for_type;
enum for_type_enum {
    FOTY_I,
    FOTY_U,
    FOTY_F
};

typedef struct for_data_struct for_data;
struct for_data_struct {
    value variable_kwrd;
    value* variable_addr;
    value start;
    value end;
    value step;
    for_type foty;
    bool is_infinite;
};

typedef struct cs_data_struct cs_data;
struct cs_data_struct {
    size_t pos;
    size_t switch_stack_size;
    size_t for_data_stack_size;
};

typedef enum def_type_enum def_type;
enum def_type_enum {
    DEF_NONE,
    DEF_CALLABLE,
    DEF_VARIABLE,
    DEF_STRUCT
};

typedef struct def_data_struct def_data;
struct def_data_struct {
    size_t data;
    def_type dety;
};

for_data sabr_interpreter_for_data_new(void);

#endif