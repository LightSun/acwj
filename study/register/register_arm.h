#ifndef _REGISTER_ARM_H
#define _REGISTER_arm_H
#include "../register_pri.h"
CPP_START

void register_arm_free_all(REGISTER_CONTEXT_PARAM);
// Print out the assembly preamble
void register_arm_cgpreamble(REGISTER_CONTEXT_PARAM);
void register_arm_cgpostamble(REGISTER_CONTEXT_PARAM, int sym_id);

int register_arm_cgloadint(REGISTER_CONTEXT_PARAM, int value, int type);
int register_arm_cgadd(REGISTER_CONTEXT_PARAM, int r1, int r2);
int register_arm_cgsub(REGISTER_CONTEXT_PARAM, int r1, int r2);
int register_arm_cgmul(REGISTER_CONTEXT_PARAM, int r1, int r2);
int register_arm_cgdiv(REGISTER_CONTEXT_PARAM, int r1, int r2);

void register_arm_cgprintint(REGISTER_CONTEXT_PARAM, int r);

// Load a value from a variable into a register.
// Return the number of the register
//name: identifier name
int register_arm_cgloadglob(REGISTER_CONTEXT_PARAM, int sym_id, int op);
// Store a register's value into a variable
//name: identifier name
int register_arm_cgstoreglob(REGISTER_CONTEXT_PARAM, int r, int sym_id);
// Generate a global symbol
//pType: one of Primitive types
//name: identifier name
void register_arm_cgglobsym(REGISTER_CONTEXT_PARAM, int sym_id);

//----------------- compare operator -------------------------
/* int register_arm_cgequal(REGISTER_CONTEXT_PARAM, int r1, int r2);
int register_arm_cgnotequal(REGISTER_CONTEXT_PARAM, int r1, int r2);
int register_arm_cglessthan(REGISTER_CONTEXT_PARAM, int r1, int r2);
int register_arm_cggreaterthan(REGISTER_CONTEXT_PARAM, int r1, int r2);

int register_arm_cglessequal(REGISTER_CONTEXT_PARAM, int r1, int r2);
int register_arm_cggreaterequal(REGISTER_CONTEXT_PARAM, int r1, int r2); */

//---------------- if statement ----------------
// Generate a jump to a label
void register_arm_cgjump(REGISTER_CONTEXT_PARAM, int l);
// Generate a label
void register_arm_cglabel(REGISTER_CONTEXT_PARAM, int l);

int register_arm_cgcompare_and_jump(REGISTER_CONTEXT_PARAM, int asTop, int r1, int r2, int label);
int register_arm_cgcompare_and_set(REGISTER_CONTEXT_PARAM, int asTop, int r1, int r2);

//-------------- function -----------------------
void register_arm_cgfuncpreamble(REGISTER_CONTEXT_PARAM, int sym_id);

void register_arm_cgfuncpostamble(REGISTER_CONTEXT_PARAM, int sym_id);

//---------------- widen --------------------------
int register_arm_cgwiden(REGISTER_CONTEXT_PARAM, int r, int oldtype, int newtype);

//get the data size of the primitive type.
int register_arm_cgprimsize(REGISTER_CONTEXT_PARAM, int pType);

/**
 * To call a function with one argument, we need to copy the register with the argument value into %rdi.
 *  On return, we need to copy the returned value from %rax into the register that will have this new value:
 * */
int register_arm_cgcall(REGISTER_CONTEXT_PARAM, int reg, int sym_id);

//return
void register_arm_cgreturn(REGISTER_CONTEXT_PARAM, int reg, int sym_id);

//get addr
int register_arm_cgaddress(REGISTER_CONTEXT_PARAM, int id);
// de - ref-addr
int register_arm_cgderef(REGISTER_CONTEXT_PARAM, int r, int pType);

// <<
int register_arm_cgshlconst(REGISTER_CONTEXT_PARAM, int r, int val);

// a = *b
int register_arm_cgstorederef(REGISTER_CONTEXT_PARAM, int leftReg, int rightReg, int type);

//gen label for store string.
void register_arm_cgglobstr(REGISTER_CONTEXT_PARAM, int label, const char *str);

//given the label id of a global string. load its address into a new register.
int register_arm_cgloadglobstr(REGISTER_CONTEXT_PARAM, int id);


// & | ^  << >>
int register_arm_cgand(REGISTER_CONTEXT_PARAM, int r1, int r2);
int register_arm_cgor(REGISTER_CONTEXT_PARAM, int r1, int r2);
int register_arm_cgxor(REGISTER_CONTEXT_PARAM, int r1, int r2);
int register_arm_cgshl(REGISTER_CONTEXT_PARAM, int r1, int r2);
int register_arm_cgshr(REGISTER_CONTEXT_PARAM, int r1, int r2);

// Negate a register's value
int register_arm_cgnegate(REGISTER_CONTEXT_PARAM, int r);
// Invert a register's value
int register_arm_cginvert(REGISTER_CONTEXT_PARAM, int r);
// Logically negate a register's value
int register_arm_cglognot(REGISTER_CONTEXT_PARAM, int r);

int register_arm_cgboolean(REGISTER_CONTEXT_PARAM, int r, int op, int label);

 //get local variable offset
//int register_arm_cggetlocaloffset(REGISTER_CONTEXT_PARAM, int type, int isParam);

//the text and data segment for asm
void register_arm_cgtextseg(REGISTER_CONTEXT_PARAM);
void register_arm_cgdataseg(REGISTER_CONTEXT_PARAM);

// Load a value from a local variable into a register.
// Return the number of the register. If the
// operation is pre- or post-increment/decrement,
// also perform this action.
int register_arm_cgloadlocal(REGISTER_CONTEXT_PARAM, int id, int op);

// Store a register's value into a local variable
int register_arm_cgstorelocal(REGISTER_CONTEXT_PARAM, int r, int id);

CPP_END
#endif