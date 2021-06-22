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

// Storage classes
enum {
        C_GLOBAL = 1,           // Globally visible symbol
        C_LOCAL                 // Locally visible symbol
};

// Symbol table structure
typedef struct _SymTable SymTable;
struct _SymTable
{
  char *name;   // Name of a symbol
  int type;     // Primitive type for the symbol
  int stype;    // Structural type for the symbol
  int class;    // Storage class for the symbol

  int endlabel; // For S_FUNCTIONs, the end label
  int size;     // Number of elements in the symbol
  int posn;     // for locals, the negative offset from the stack base pointer.
};

struct _GlobalContext;

/*
0xxxx......................................xxxxxxxxxxxxNSYMBOLS-1
     ^                                    ^
     |                                    |
   Globs                                Locls
   */
struct GlobalState
{
  SymTable *syms; // Global symbol table array. size is SYM_BOLS_COUNT
  int globs;      // Position of next free global symbol slot
  int locals;      // Position of next free local  symbol slot

  struct _GlobalContext *gContext;
};

struct GlobalState *sym_globalState_new();
void sym_globalState_delete(struct GlobalState *gs);

int sym_findglob(struct GlobalState *gs, const char *s);
int sym_findLocal(struct GlobalState *gs, const char *s);
// Determine if the symbol s is in the symbol table.
// Return its slot position or -1 if not found.
int sym_findSymbol(struct GlobalState *gs, const char *s);

// Add a global symbol to the symbol table. Set up its:
// + name: the identifier name
// + type: char, int etc.
// + structural type: var, function, array etc.
// + endlabel: if this is a function
// + size: number of elements
// Return the slot number in the symbol table
int sym_addglob(struct GlobalState *gs, const char *name, int type, int stype, int endlabel, int size);

// Add a local symbol to the symbol table. Set up its:
// + type: char, int etc.
// + structural type: var, function, array etc.
// + size: number of elements
// + endlabel: if this is a function
// Return the slot number in the symbol table
int sym_addlocal(struct GlobalState *gs, const char *name, int type, int stype, int endlabel, int size);

SymTable *sym_getSymbol(struct GlobalState *gs, int pos);

#define SYM_NAME(gs, id) sym_getSymbol(gs, id)->name
#define SYM_END_LABEL(gs, id) sym_getSymbol(gs, id)->endlabel
#define SYM_TYPE(gs, id) sym_getSymbol(gs, id)->type
#define SYM_STYPE(gs, id) sym_getSymbol(gs, id)->stype
#define SYM_IS_LOCAL(gs, id) sym_getSymbol(gs, id)->class==C_LOCAL

CPP_END

#endif