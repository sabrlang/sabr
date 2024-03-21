#ifndef __INTERPRETER_DATA_H__
#define __INTERPRETER_DATA_H__

#include <stdbool.h>
#include <stddef.h>

#include "value.h"

#include "cctl/rbt.h"

typedef enum sabr_for_type_enum sabr_for_type_t;
enum sabr_for_type_enum {
    SABR_FOTY_I,
    SABR_FOTY_U,
    SABR_FOTY_F
};

typedef struct for_data_struct sabr_for_data_t;
struct for_data_struct {
    sabr_value_t variable_kwrd;
    sabr_value_t* variable_addr;
    sabr_value_t start;
    sabr_value_t end;
    sabr_value_t step;
    sabr_for_type_t foty;
    bool is_infinite;
};

typedef struct sabr_cs_data_struct sabr_cs_data_t;
struct sabr_cs_data_struct {
    size_t pos;
};

typedef enum sabr_def_type_enum sabr_def_type_t;
enum sabr_def_type_enum {
    SABR_DETY_NONE,
    SABR_DETY_CALLABLE,
    SABR_DETY_VARIABLE,
    SABR_DETY_STRUCT
};

typedef struct def_data_struct sabr_def_data_t;
struct def_data_struct {
    size_t data;
    sabr_def_type_t dety;
};

typedef struct sabr_local_data_struct sabr_local_data_t;
struct sabr_local_data_struct {
    size_t switch_stack_size;
    size_t for_data_stack_size;
    size_t local_memory_size;
    void* local_words;
};

sabr_for_data_t sabr_interpreter_for_data_new(void);

#endif