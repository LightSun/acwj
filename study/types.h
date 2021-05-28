
#ifndef _TYPES_H
#define _TYPES_H
#include "common_headers.h"
CPP_START
// Given two primitive types,
// return true if they are compatible,
// false otherwise. Also return either
// zero or an A_WIDEN operation if one
// has to be widened to match the other.
// If onlyright is true, only widen left to right.
int types_compatible(int *left, int *right, int onlyright);

CPP_END

#endif