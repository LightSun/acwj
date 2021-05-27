#ifndef _SYMM_H_
#define _SYMM_H_
#include "common_headers.h"

CPP_START

#define SYM_BOLS_COUNT        1024     // Number of symbol table entries

// Structural types
enum {
  S_VARIABLE, S_FUNCTION
};

// Symbol table structure
typedef struct _SymTable SymTable;
struct _SymTable {
  char *name;                   // Name of a symbol
  int type;                     // Primitive type for the symbol
  int stype;                    // Structural type for the symbol
};

int sym_findglob(char *s);
int sym_addglob(char *name, int type, int stype);
SymTable* sym_getGlob(int pos);

CPP_END

#endif