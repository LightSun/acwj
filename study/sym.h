#ifndef _SYMM_H_
#define _SYMM_H_
#include "common_headers.h"

CPP_START

#define SYM_BOLS_COUNT        1024            // Number of symbol table entries
//extern_ struct symtable Gsym[NSYMBOLS]; // Global symbol table
//static int Globs = 0;                   // Position of next free global symbol slot

// Symbol table structure
typedef struct _SymTable SymTable;
struct _SymTable {
  char *name;                   // Name of a symbol
};

int sym_findglob(char *s);
int sym_addglob(char *name);
SymTable* sym_getGlob(int pos);

CPP_END

#endif