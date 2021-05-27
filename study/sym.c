#include "sym.h"


// Symbol table functions
// Copyright (c) 2019 Warren Toomey, GPL3

static int Globs = 0;		               // Position of next free global symbol slot
static SymTable syms[SYM_BOLS_COUNT];  // Global symbol table

// Determine if the symbol s is in the global symbol table.
// Return its slot position or -1 if not found.
int sym_findglob(char *s) {
  int i;

  for (i = 0; i < Globs; i++) {
    if (*s == *syms[i].name && !strcmp(s, syms[i].name))
      return (i);
  }
  return (-1);
}

// Get the position of a new global symbol slot, or die
// if we've run out of positions.
static int newglob(void) {
  int p;

  if ((p = Globs++) >= SYM_BOLS_COUNT){
    fprintf(stderr, "Too many global symbols");
    exit(1);
  }
  return (p);
}

// Add a global symbol to the symbol table.
// Also set up its type and structural type.
// Return the slot number in the symbol table
int sym_addglob(char *name, int type, int stype) {
  int y;

  // If this is already in the symbol table, return the existing slot
  if ((y = sym_findglob(name)) != -1)
    return (y);

  // Otherwise get a new slot, fill it in and
  // return the slot number
  y = newglob();
  syms[y].name = strdup(name); //copy string. need free
  syms[y].type = type;
  syms[y].stype = stype;
  return (y);
}

SymTable* sym_getGlob(int pos){
    return &syms[pos];
}