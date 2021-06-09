#ifndef _SYMM_H_
#define _SYMM_H_
#include "common_headers.h"

CPP_START

#define SYM_BOLS_COUNT 1024 // Number of symbol table entries

// Structural types
enum
{
  S_VARIABLE,
  S_FUNCTION,
  S_ARRAY
};

// Symbol table structure
typedef struct _SymTable SymTable;
struct _SymTable
{
  char *name;   // Name of a symbol
  int type;     // Primitive type for the symbol
  int stype;    // Structural type for the symbol
  int endlabel; // For S_FUNCTIONs, the end label
  int size;     // Number of elements in the symbol
};

struct _GlobalContext;

struct GlobalState
{
  SymTable *syms; // Global symbol table array. size is SYM_BOLS_COUNT
  int globs;      // Position of next free global symbol slot

  struct _GlobalContext *gContext;
};

struct GlobalState *sym_globalState_new();
void sym_globalState_delete(struct GlobalState *gs);

int sym_findglob(struct GlobalState *gs, const char *s);

// Add a global symbol to the symbol table. Set up its:
// + name: the identifier name
// + type: char, int etc.
// + structural type: var, function, array etc.
// + endlabel: if this is a function
// + size: number of elements
// Return the slot number in the symbol table
int sym_addglob(struct GlobalState *gs, const char *name, int type, int stype, int endlabel, int size);

SymTable *sym_getGlob(struct GlobalState *gs, int pos);

#define SYM_NAME(gs, id) sym_getGlob(gs, id)->name
#define SYM_END_LABEL(gs, id) sym_getGlob(gs, id)->endlabel
#define SYM_TYPE(gs, id) sym_getGlob(gs, id)->type
#define SYM_STYPE(gs, id) sym_getGlob(gs, id)->stype

CPP_END

#endif