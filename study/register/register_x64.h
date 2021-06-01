#ifndef _REGISTER_X64_H
#define _REGISTER_X64_H
#include "../register_pri.h"
CPP_START
// Print out the assembly preamble
void register_x64_cgpreamble(REGISTER_CONTEXT_PARAM);
void register_x64_cgpostamble(REGISTER_CONTEXT_PARAM, int sym_id);

int register_x64_cgloadint(REGISTER_CONTEXT_PARAM, int value);
int register_x64_cgadd(REGISTER_CONTEXT_PARAM, int r1, int r2);
int register_x64_cgsub(REGISTER_CONTEXT_PARAM, int r1, int r2);
int register_x64_cgmul(REGISTER_CONTEXT_PARAM, int r1, int r2);
int register_x64_cgdiv(REGISTER_CONTEXT_PARAM, int r1, int r2);

void register_x64_cgprintint(REGISTER_CONTEXT_PARAM, int r);

// Load a value from a variable into a register.
// Return the number of the register
//name: identifier name
int register_x64_cgloadglob(REGISTER_CONTEXT_PARAM, int sym_id);
// Store a register's value into a variable
//name: identifier name
int register_x64_cgstoreglob(REGISTER_CONTEXT_PARAM, int r, int sym_id);
// Generate a global symbol
//pType: one of Primitive types
//name: identifier name
void register_x64_cgglobsym(REGISTER_CONTEXT_PARAM, int sym_id);

void register_x64_free_all(REGISTER_CONTEXT_PARAM);

//----------------- compare operator -------------------------
int register_x64_cgequal(REGISTER_CONTEXT_PARAM, int r1, int r2);
int register_x64_cgnotequal(REGISTER_CONTEXT_PARAM, int r1, int r2);
int register_x64_cglessthan(REGISTER_CONTEXT_PARAM, int r1, int r2);
int register_x64_cggreaterthan(REGISTER_CONTEXT_PARAM, int r1, int r2);

int register_x64_cglessequal(REGISTER_CONTEXT_PARAM, int r1, int r2);
int register_x64_cggreaterequal(REGISTER_CONTEXT_PARAM, int r1, int r2);

//---------------- if statement ----------------
// Generate a jump to a label
void register_x64_cgjump(REGISTER_CONTEXT_PARAM, int l);
// Generate a label
void register_x64_cglabel(REGISTER_CONTEXT_PARAM, int l);

int register_x64_cgcompare_and_jump(REGISTER_CONTEXT_PARAM, int asTop, int r1, int r2, int label);
int register_x64_cgcompare_and_set(REGISTER_CONTEXT_PARAM, int asTop, int r1, int r2);

//-------------- function -----------------------
void register_x64_cgfuncpreamble(REGISTER_CONTEXT_PARAM, int sym_id);

void register_x64_cgfuncpostamble(REGISTER_CONTEXT_PARAM, int sym_id);

//---------------- widen --------------------------
int register_x64_cgwiden(REGISTER_CONTEXT_PARAM, int r, int oldtype, int newtype);

//get the data size of the primitive type.
int register_x64_cgprimsize(REGISTER_CONTEXT_PARAM, int pType);

/**
 * To call a function with one argument, we need to copy the register with the argument value into %rdi.
 *  On return, we need to copy the returned value from %rax into the register that will have this new value:
 * */
int register_x64_cgcall(REGISTER_CONTEXT_PARAM, int reg, int sym_id);

//return
void register_x64_cgreturn(REGISTER_CONTEXT_PARAM, int reg, int sym_id);

//get addr
int register_x64_cgaddress(REGISTER_CONTEXT_PARAM, int id);

// de - ref-addr
int register_x64_cgderef(REGISTER_CONTEXT_PARAM, int r, int pType);

CPP_END
#endif