#include "sym.h"
#include "gen.h"
#include "writer.h"

struct GlobalState* sym_globalState_new(){
  struct GlobalState* gs = (struct GlobalState*)malloc(sizeof(struct GlobalState));
  gs->globs = 0;
  gs->locals = SYM_BOLS_COUNT - 1;
  gs->syms = (SymTable*)malloc(SYM_BOLS_COUNT * sizeof(SymTable));
  memset(gs->syms, 0, SYM_BOLS_COUNT * sizeof(SymTable));
  return gs;
}
void sym_globalState_delete(struct GlobalState* gs){
  //gs->globs = 0;
  for (size_t i = 0; i < SYM_BOLS_COUNT; i++)
  {
    if(gs->syms[i].name){
      free(gs->syms[i].name);
      gs->syms[i].name = NULL;
    }
  }
  
  free(gs->syms);
  free(gs);
}

// Determine if the symbol s is in the global symbol table.
// Return its slot position or -1 if not found.
int sym_findglob(struct GlobalState* gs, const char *s) {
  int i;
  for (i = 0; i < gs->globs; i++) {
      if (gs->syms[i].class == C_PARAM) continue;
    if (*s == *(gs->syms[i].name) && !strcmp(s, gs->syms[i].name))
      return (i);
  }
  return (-1);
}

int sym_findLocal(struct GlobalState* gs, const char *s) {
  int i;

  for (i = gs->locals + 1; i < SYM_BOLS_COUNT; i++) {
     if (*s == *(gs->syms[i].name) && !strcmp(s, gs->syms[i].name))
      return (i);
  }
  return (-1);
}

// Determine if the symbol s is in the symbol table.
// Return its slot position or -1 if not found.
int sym_findSymbol(struct GlobalState* gs, const char *s) {
  int slot;

  slot = sym_findLocal(gs, s);
  if (slot == -1)
    slot = sym_findglob(gs, s);
  return (slot);
}

// Get the position of a new global symbol slot, or die
// if we've run out of positions.
static int newglob(struct GlobalState* gs) {
  int p;

  if ((p = gs->globs++) >= gs->locals){
    fprintf(stderr, "Too many global symbols");
    exit(1);
  }
  return (p);
}
static int newLocal(struct GlobalState* gs) {
  int p;

//0, -1, -2, -3 ,-4
  if ((p = gs->locals--) <= gs->globs){
    fprintf(stderr, "Too many local symbols");
    exit(1);
  }
  return (p);
}
// Update a symbol at the given slot number in the symbol table. Set up its:
// + type: char, int etc.
// + structural type: var, function, array etc.
// + size: number of elements
// + endlabel: if this is a function
// + posn: Position information for local symbols
// + class: local or globals
static void updatesym(struct GlobalState* gs,int slot, const char *name, int type, int stype,
		      int class, int endlabel, int size, int posn) {
  if (slot < 0 || slot >= SYM_BOLS_COUNT){
    fprintf(stderr, "Invalid symbol slot number in updatesym()");
    exit(1);
  }
  gs->syms[slot].name = strdup(name); //copy string. need free
  gs->syms[slot].type = type;
  gs->syms[slot].stype = stype;
  gs->syms[slot].class = class;
  gs->syms[slot].endlabel = endlabel;
  gs->syms[slot].size = size;
  gs->syms[slot].posn = posn;
}

int sym_addglob(struct GlobalState* gs, const char *name, int type, int stype, int endlabel, int size)
{
  int slot;

  // If this is already in the symbol table, return the existing slot
  if ((slot = sym_findglob(gs, name)) != -1)
    return (slot);

  // Otherwise get a new slot, fill it in and
  // return the slot number
  slot = newglob(gs);
  updatesym(gs, slot, name, type, stype, C_GLOBAL, endlabel, size, 0);
  gen_globsym(gs->gContext->writer, slot);
  return (slot);
}

int sym_addlocal(struct GlobalState* gs, const char *name, int type, int stype, int isParam, int size) {
  int localslot, globalslot;

  // If this is already in the symbol table, return the existing slot
  if ((localslot = sym_findLocal(gs, name)) != -1)
    return (localslot);

  // Otherwise get a new symbol slot and a position for this local.
  // Update the local symbol table entry. If this is a parameter,
  // also create a global C_PARAM entry to build the function's prototype.
  localslot = newLocal(gs);
  if(isParam){
      updatesym(gs, localslot, name, type, stype, C_PARAM, 0, size, 0);
      globalslot = newglob(gs);
      updatesym(gs, globalslot, name, type, stype, C_PARAM, 0, size, 0);
  }else{
     updatesym(gs, localslot, name, type, stype, C_LOCAL, 0, size, 0);
  } 
  return (localslot);
}

SymTable* sym_getSymbol(struct GlobalState* gs, int pos){
    return &gs->syms[pos];
}

void sym_freeloclsyms(struct GlobalState *gs){
  gs->locals = SYM_BOLS_COUNT - 1;
}