#include "common_headers.h"
#ifndef _REGISTER_H
#define _REGISTER_H
#include "writer.h"
CPP_START

#define REGISTER_CONTEXT_PARAM Writer *w

// Print out the assembly preamble
void register_cgpreamble(REGISTER_CONTEXT_PARAM);
void register_cgpostamble(REGISTER_CONTEXT_PARAM);

int register_cgload(REGISTER_CONTEXT_PARAM, int value);
int register_cgadd(REGISTER_CONTEXT_PARAM, int r1, int r2);
int register_cgsub(REGISTER_CONTEXT_PARAM, int r1, int r2);
int register_cgmul(REGISTER_CONTEXT_PARAM, int r1, int r2);
int register_cgdiv(REGISTER_CONTEXT_PARAM, int r1, int r2);

void register_cgprintint(REGISTER_CONTEXT_PARAM, int r);

// Load a value from a variable into a register.
// Return the number of the register
int register_cgloadglob(REGISTER_CONTEXT_PARAM, const char* identifier);
// Store a register's value into a variable
int register_cgstoreglob(REGISTER_CONTEXT_PARAM, int r, const char* identifier);
// Generate a global symbol
void register_cgglobsym(REGISTER_CONTEXT_PARAM, int id, const char* sym);

void register_free_all();

//----------------- compare operator -------------------------
int register_cgequal(REGISTER_CONTEXT_PARAM, int r1, int r2);
int register_cgnotequal(REGISTER_CONTEXT_PARAM, int r1, int r2);
int register_cglessthan(REGISTER_CONTEXT_PARAM, int r1, int r2);
int register_cggreaterthan(REGISTER_CONTEXT_PARAM, int r1, int r2);

int register_cglessequal(REGISTER_CONTEXT_PARAM, int r1, int r2);
int register_cggreaterequal(REGISTER_CONTEXT_PARAM, int r1, int r2);


//---------------- if statement ----------------
// Generate a jump to a label
void register_cgjump(REGISTER_CONTEXT_PARAM, int l);
// Generate a label
void register_cglabel(REGISTER_CONTEXT_PARAM, int l);

int register_cgcompare_and_jump(REGISTER_CONTEXT_PARAM, int asTop, int r1, int r2, int label);
int register_cgcompare_and_set(REGISTER_CONTEXT_PARAM, int asTop,int r1, int r2);

//-------------- function -----------------------
void register_cgfuncpreamble(REGISTER_CONTEXT_PARAM, const char* funcName);

void register_cgfuncpostamble(REGISTER_CONTEXT_PARAM);

CPP_END

#endif