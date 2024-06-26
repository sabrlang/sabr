#ifndef __VALUE_H__
#define __VALUE_H__

#include <stdint.h>

typedef union sabr_value_union {
	int64_t i;
	uint64_t u;
	double f;
	uint64_t* p;
	uint8_t bytes[8];
// #if defined(__GNUC__) || defined(__INTEL_COMPILER)
// 	_Decimal64 d;
// #endif
} sabr_value_t;

sabr_value_t sabr_value_zero();

#endif