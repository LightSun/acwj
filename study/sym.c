#include "sym.h"

struct GlobalState* sym_globalState_new(){
  struct GlobalState* gs = (struct GlobalState*)malloc(sizeof(struct GlobalState));
  gs->globs = 0;
  gs->syms = (SymTable*)malloc(SYM_BOLS_COUNT * sizeof(SymTable));
  memset(gs->syms, 0, SYM_BOLS_COUNT * sizeof(SymTable));
  return gs;
}
void sym_globalState_delete(struct GlobalState* gs){
  //gs->globs = 0;
  free(gs->syms);
  free(gs);
}

// Determine if the symbol s is in the global symbol table.
// Return its slot position or -1 if not found.
int sym_findglob(struct GlobalState* gs, const char *s) {
  int i;
  for (i = 0; i < gs->globs; i++) {
    if (*s == *(gs->syms[i].name) && !strcmp(s, gs->syms[i].name))
      return (i);
  }
  return (-1);
}

// Get the position of a new global symbol slot, or die
// if we've run out of positions.
static int newglob(struct GlobalState* gs) {
  int p;

  if ((p = gs->globs++) >= SYM_BOLS_COUNT){
    fprintf(stderr, "Too many global symbols");
    exit(1);
  }
  return (p);
}

// Add a global symbol to the symbol table.
// Also set up its type and structural type.
// Return the slot number in the symbol table
int sym_addglob(struct GlobalState* gs, const char *name, int type, int stype, int endlabel)
{
  int y;

  // If this is already in the symbol table, return the existing slot
  if ((y = sym_findglob(gs, name)) != -1)
    return (y);

  // Otherwise get a new slot, fill it in and
  // return the slot number
  y = newglob(gs);
  gs->syms[y].name = strdup(name); //copy string. need free
  gs->syms[y].type = type;
  gs->syms[y].stype = stype;
  gs->syms[y].endlabel = endlabel;
  return (y);
}

SymTable* sym_getGlob(struct GlobalState* gs, int pos){
    return &gs->syms[pos];
}