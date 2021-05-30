#ifndef _REGISTER_ARM64_H
#define _REGISTER_ARM64_H
#include "../register_pri.h"
CPP_START

void register_arm64_free_all(REGISTER_CONTEXT_PARAM);
// Print out the assembly preamble
void register_arm64_cgpreamble(REGISTER_CONTEXT_PARAM);
void register_arm64_cgpostamble(REGISTER_CONTEXT_PARAM, int endLabel);

int register_arm64_cgload(REGISTER_CONTEXT_PARAM, int value);
int register_arm64_cgadd(REGISTER_CONTEXT_PARAM, int r1, int r2);
int register_arm64_cgsub(REGISTER_CONTEXT_PARAM, int r1, int r2);
int register_arm64_cgmul(REGISTER_CONTEXT_PARAM, int r1, int r2);
int register_arm64_cgdiv(REGISTER_CONTEXT_PARAM, int r1, int r2);

void register_arm64_cgprintint(REGISTER_CONTEXT_PARAM, int r);

// Load a value from a variable into a register.
// Return the number of the register
//name: identifier name
int register_arm64_cgloadglob(REGISTER_CONTEXT_PARAM, int pType, const char *name);
// Store a register's value into a variable
//name: identifier name
int register_arm64_cgstoreglob(REGISTER_CONTEXT_PARAM, int r, int pType, const char *name);
// Generate a global symbol
//pType: one of Primitive types
//name: identifier name
void register_arm64_cgglobsym(REGISTER_CONTEXT_PARAM, int pType, const char *name);

//----------------- compare operator -------------------------
int register_arm64_cgequal(REGISTER_CONTEXT_PARAM, int r1, int r2);
int register_arm64_cgnotequal(REGISTER_CONTEXT_PARAM, int r1, int r2);
int register_arm64_cglessthan(REGISTER_CONTEXT_PARAM, int r1, int r2);
int register_arm64_cggreaterthan(REGISTER_CONTEXT_PARAM, int r1, int r2);

int register_arm64_cglessequal(REGISTER_CONTEXT_PARAM, int r1, int r2);
int register_arm64_cggreaterequal(REGISTER_CONTEXT_PARAM, int r1, int r2);

//---------------- if statement ----------------
// Generate a jump to a label
void register_arm64_cgjump(REGISTER_CONTEXT_PARAM, int l);
// Generate a label
void register_arm64_cglabel(REGISTER_CONTEXT_PARAM, int l);

int register_arm64_cgcompare_and_jump(REGISTER_CONTEXT_PARAM, int asTop, int r1, int r2, int label);
int register_arm64_cgcompare_and_set(REGISTER_CONTEXT_PARAM, int asTop, int r1, int r2);

//-------------- function -----------------------
void register_arm64_cgfuncpreamble(REGISTER_CONTEXT_PARAM, const char *funcName);

void register_arm64_cgfuncpostamble(REGISTER_CONTEXT_PARAM, int endLabel);

//---------------- widen --------------------------
int register_arm64_cgwiden(REGISTER_CONTEXT_PARAM, int r, int oldtype, int newtype);

//get the data size of the primitive type.
int register_arm64_cgprimsize(REGISTER_CONTEXT_PARAM, int pType);

/**
 * To call a function with one argument, we need to copy the register with the argument value into %rdi.
 *  On return, we need to copy the returned value from %rax into the register that will have this new value:
 * */
int register_arm64_cgcall(REGISTER_CONTEXT_PARAM, int reg, const char *symName);

//return
void register_arm64_cgreturn(REGISTER_CONTEXT_PARAM, int reg, int pType, int endlabel);

CPP_END
#endif