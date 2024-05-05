#include "bif.h"

#include "bif_math.h"
#include "bif_io.h"

sabr_bif_func_t* sabr_bif_modules[] = {
	sabr_bif_math_functions,
	sabr_bif_io_functions
};
size_t sabr_bif_modules_len = sizeof(sabr_bif_modules) / sizeof(sabr_bif_func_t*);