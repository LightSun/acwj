
#ifndef _TYPES_H
#define _TYPES_H
#include "common_headers.h"
CPP_START

struct _Writer;
struct _Content;

// Given two primitive types,
// return true if they are compatible,
// false otherwise. Also return either
// zero or an A_WIDEN operation if one
// has to be widened to match the other.
// If onlyright is true, only widen left to right.
int types_compatible(struct _Writer* w, int *left, int *right, int onlyright);

int types_parse_type(struct _Content *cd, struct Token *token);

int types_pointer_to(int type);

int types_value_at(int type);

struct ASTnode *types_modify_type(struct ASTnode *tree, struct _Writer *w, int rtype, int op);

CPP_END

#endif