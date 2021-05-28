#include "register.h"
#include "ast.h"
// Code generator for x86-64
// Copyright (c) 2019 Warren Toomey, GPL3

// List of available registers and their names
// We need a list of byte registers, too
static int freereg[4];
static char *reglist[4] = {"%r8", "%r9", "%r10", "%r11"};      //long
static char *breglist[4] = {"%r8b", "%r9b", "%r10b", "%r11b"}; //char
static char *dreglist[4] = {"%r8d", "%r9d", "%r10d", "%r11d"}; //int

// Set all registers as available
void register_free_all(void)
{
  freereg[0] = freereg[1] = freereg[2] = freereg[3] = 1;
}

// Allocate a free register. Return the number of
// the register. Die if no available registers.
static int register_alloc(void)
{
  for (int i = 0; i < 4; i++)
  {
    if (freereg[i])
    {
      freereg[i] = 0;
      return (i);
    }
  }
  fprintf(stderr, "Out of registers!\n");
  exit(1);
}

// Return a register to the list of available registers.
// Check to see if it's not already there.
static void register_free(int reg)
{
  if (freereg[reg] != 0)
  {
    fprintf(stderr, "Error trying to free register %d\n", reg);
    exit(1);
  }
  freereg[reg] = 1;
}

// Print out the assembly preamble
void register_cgpreamble(REGISTER_CONTEXT_PARAM)
{
  register_free_all();
  w->writeChars(w->context,"\t.text\n");
}

// Print out the assembly postamble
void register_cgpostamble(REGISTER_CONTEXT_PARAM, int endLabel)
{
  
  register_cglabel(w, endLabel);
  //fputs("\tpopq %rbp\n" "\tret\n", Outfile);
  w->writeChars(w->context, "\tpopq %rbp\n"
                            "\tret\n");
}

// Load an integer literal value into a register.
// Return the number of the register
int register_cgload(REGISTER_CONTEXT_PARAM, int value)
{

  // Get a new register
  int r = register_alloc();

  // Print out the code to initialise it
  //fprintf(Outfile, "\tmovq\t$%d, %s\n", value, reglist[r]);
  char buffer[32];
  snprintf(buffer, 32, "\tmovq\t$%d, %s\n", value, reglist[r]);
  w->writeChars(w->context, buffer);
  return (r);
}

// Add two registers together and return
// the number of the register with the result
int register_cgadd(REGISTER_CONTEXT_PARAM, int r1, int r2)
{
  //fprintf(Outfile, "\taddq\t%s, %s\n", reglist[r1], reglist[r2]);
  char buffer[32];
  snprintf(buffer, 32, "\taddq\t%s, %s\n", reglist[r1], reglist[r2]);
  w->writeChars(w->context, buffer);

  register_free(r1);
  return (r2);
}

// Subtract the second register from the first and
// return the number of the register with the result
int register_cgsub(REGISTER_CONTEXT_PARAM, int r1, int r2)
{
  //fprintf(Outfile, "\tsubq\t%s, %s\n", reglist[r2], reglist[r1]);
  char buffer[32];
  snprintf(buffer, 32, "\tsubq\t%s, %s\n", reglist[r2], reglist[r1]);
  w->writeChars(w->context, buffer);

  register_free(r2);
  return (r1);
}

// Multiply two registers together and return
// the number of the register with the result
int register_cgmul(REGISTER_CONTEXT_PARAM, int r1, int r2)
{
  //fprintf(Outfile, "\timulq\t%s, %s\n", reglist[r1], reglist[r2]);
  char buffer[32];
  snprintf(buffer, 32, "\timulq\t%s, %s\n", reglist[r1], reglist[r2]);
  w->writeChars(w->context, buffer);

  register_free(r1);
  return (r2);
}

// Divide the first register by the second and
// return the number of the register with the result
int register_cgdiv(REGISTER_CONTEXT_PARAM, int r1, int r2)
{
  char buffer[32];
  snprintf(buffer, 32, "\tmovq\t%s,%%rax\n", reglist[r1]);
  w->writeChars(w->context, buffer);

  w->writeChars(w->context, "\tcqo\n");

  snprintf(buffer, 32, "\tidivq\t%s\n", reglist[r2]);
  w->writeChars(w->context, buffer);

  snprintf(buffer, 32, "\tmovq\t%%rax,%s\n", reglist[r1]);
  w->writeChars(w->context, buffer);

  /*  fprintf(Outfile, "\tmovq\t%s,%%rax\n", reglist[r1]);
  fprintf(Outfile, "\tcqo\n");
  fprintf(Outfile, "\tidivq\t%s\n", reglist[r2]);
  fprintf(Outfile, "\tmovq\t%%rax,%s\n", reglist[r1]); */
  register_free(r2);
  return (r1);
}

// Call printint() with the given register
void register_cgprintint(REGISTER_CONTEXT_PARAM, int r)
{
  /* fprintf(Outfile, "\tmovq\t%s, %%rdi\n", reglist[r]);
  fprintf(Outfile, "\tcall\tprintint\n"); */

  char buffer[32];
  snprintf(buffer, 32, "\tmovq\t%s, %%rdi\n", reglist[r]);
  w->writeChars(w->context, buffer);

  w->writeChars(w->context, "\tcall\tprintint\n");

  register_free(r);
}

//------------- from class 6 -------------------------
int register_cgloadglob(REGISTER_CONTEXT_PARAM, int pType, const char *name)
{
  // Get a new register
  int r = register_alloc();

  char buffer[64];
  // Print out the code to initialise it
  switch (pType)
  {
  case P_CHAR:
    //fprintf(Outfile, "\tmovzbq\t%s(\%%rip), %s\n", Gsym[id].name, reglist[r]);
    snprintf(buffer, 64, "\tmovzbq\t%s(\%%rip), %s\n", name, reglist[r]);
    break;

  case P_INT:
    //fprintf(Outfile, "\tmovzbl\t%s(\%%rip), %s\n", Gsym[id].name, reglist[r]);
    snprintf(buffer, 64, "\tmovzbl\t%s(\%%rip), %s\n", name, reglist[r]);
    break;

  case P_LONG:
    //fprintf(Outfile, "\tmovq\t%s(\%%rip), %s\n", Gsym[id].name, reglist[r]);
    snprintf(buffer, 64, "\tmovq\t%s(\%%rip), %s\n", name, reglist[r]);
    break;

  default:
    fprintf(stderr, "Bad type in register_cgloadglob:", pType);
    exit(1);
  }
  w->writeChars(w->context, buffer);
  return (r);
}

// Store a register's value into a variable
int register_cgstoreglob(REGISTER_CONTEXT_PARAM, int r, int pType, const char *name)
{
  char buffer[64];
  switch (pType)
  {
  case P_CHAR:
    // fprintf(Outfile, "\tmovb\t%s, %s(\%%rip)\n", breglist[r], Gsym[id].name);
    snprintf(buffer, 64, "\tmovb\t%s, %s(\%%rip)\n", breglist[r], name);
    break;

  case P_INT:
    // fprintf(Outfile, "\tmovl\t%s, %s(\%%rip)\n", dreglist[r], Gsym[id].name);
    snprintf(buffer, 64, "\tmovl\t%s, %s(\%%rip)\n", dreglist[r], name);
    break;

  case P_LONG:
    //fprintf(Outfile, "\tmovq\t%s, %s(\%%rip)\n", reglist[r], Gsym[id].name);
    snprintf(buffer, 64, "\tmovq\t%s, %s(\%%rip)\n", reglist[r], name);
    break;

  default:
    fprintf(stderr, "Bad type in register_cgstoreglob:", pType);
    exit(1);
  }
  w->writeChars(w->context, buffer);
  return (r);
}

// Generate a global symbol
void register_cgglobsym(REGISTER_CONTEXT_PARAM, int pType, const char *sym)
{
  int typesize;
  typesize = register_cgprimsize(w, pType);
  //fprintf(Outfile, "\t.comm\t%s,%d,%d\n", Gsym[id].name, typesize, typesize);

  char buffer[32];
  snprintf(buffer, 32, "\t.comm\t%s,%d,%d\n", sym, typesize, typesize);

  w->writeChars(w->context, buffer);
}

//--------------------- compare operator -------------------
//x86_64 registers: https://github.com/LightSun/acwj/tree/master/07_Comparisons
// Compare two registers.
static int cgcompare(REGISTER_CONTEXT_PARAM, int r1, int r2, char *how)
{

  /* fprintf(Outfile, "\tcmpq\t%s, %s\n", reglist[r2], reglist[r1]);
  fprintf(Outfile, "\t%s\t%s\n", how, breglist[r2]);
  fprintf(Outfile, "\tandq\t$255,%s\n", reglist[r2]); 
  free_register(r1); */

  char buffer[32];
  snprintf(buffer, 32, "\tcmpq\t%s, %s\n", reglist[r2], reglist[r1]);
  w->writeChars(w->context, buffer);

  snprintf(buffer, 32, "\t%s\t%s\n", how, breglist[r2]);
  w->writeChars(w->context, buffer);

  snprintf(buffer, 32, "\tandq\t$255,%s\n", reglist[r2]);
  w->writeChars(w->context, buffer);

  register_free(r1);
  return (r2);
}

int register_cgequal(REGISTER_CONTEXT_PARAM, int r1, int r2)
{
  return (cgcompare(w, r1, r2, "sete"));
}
int register_cgnotequal(REGISTER_CONTEXT_PARAM, int r1, int r2)
{
  return (cgcompare(w, r1, r2, "setne"));
}
int register_cglessthan(REGISTER_CONTEXT_PARAM, int r1, int r2)
{
  return (cgcompare(w, r1, r2, "setl"));
}
int register_cggreaterthan(REGISTER_CONTEXT_PARAM, int r1, int r2)
{
  return (cgcompare(w, r1, r2, "setg"));
}
int register_cglessequal(REGISTER_CONTEXT_PARAM, int r1, int r2)
{
  return (cgcompare(w, r1, r2, "setle"));
}
int register_cggreaterequal(REGISTER_CONTEXT_PARAM, int r1, int r2)
{
  return (cgcompare(w, r1, r2, "setge"));
}

//------------------------- if statements ---------------------------

void register_cgjump(REGISTER_CONTEXT_PARAM, int l)
{
  // fprintf(Outfile, "\tjmp\tL%d\n", l);
  char buffer[32];
  snprintf(buffer, 32, "\tjmp\tL%d\n", l);
  w->writeChars(w->context, buffer);
}
void register_cglabel(REGISTER_CONTEXT_PARAM, int l)
{
  // fprintf(Outfile, "L%d:\n", l);
  char buffer[32];
  snprintf(buffer, 32, "L%d:\n", l);
  w->writeChars(w->context, buffer);
}

// List of inverted jump instructions,
// = -> !=, != -> =, < -> >=, > -> <=, <= -> >, >= -> <
// in AST order: A_EQ, A_NE, A_LT, A_GT, A_LE, A_GE
static char *invcmplist[] = {"jne", "je", "jge", "jle", "jg", "jl"};

// Compare two registers and jump if false.
int register_cgcompare_and_jump(REGISTER_CONTEXT_PARAM, int ASTop, int r1, int r2, int label)
{

  // Check the range of the AST operation
  if (ASTop < A_EQ || ASTop > A_GE)
  {
    fprintf(stderr, "Bad ASTop in register_cgcompare_and_jump()");
    exit(1);
  }

  /*  fprintf(Outfile, "\tcmpq\t%s, %s\n", reglist[r2], reglist[r1]);
  fprintf(Outfile, "\t%s\tL%d\n", invcmplist[ASTop - A_EQ], label);
  freeall_registers(); */

  char buffer[32];
  snprintf(buffer, 32, "\tcmpq\t%s, %s\n", reglist[r2], reglist[r1]);
  w->writeChars(w->context, buffer);

  snprintf(buffer, 32, "\t%s\tL%d\n", invcmplist[ASTop - A_EQ], label);
  w->writeChars(w->context, buffer);

  register_free_all();
  return (NOREG);
}

// List of comparison instructions,
// in AST order: A_EQ, A_NE, A_LT, A_GT, A_LE, A_GE
static char *cmplist[] =
    {"sete", "setne", "setl", "setg", "setle", "setge"};

// Compare two registers and set if true.
int register_cgcompare_and_set(REGISTER_CONTEXT_PARAM, int ASTop, int r1, int r2)
{

  // Check the range of the AST operation
  if (ASTop < A_EQ || ASTop > A_GE)
  {
    fprintf(stderr, "Bad ASTop in cgcompare_and_set()");
    exit(1);
  }

  /* fprintf(Outfile, "\tcmpq\t%s, %s\n", reglist[r2], reglist[r1]);
  fprintf(Outfile, "\t%s\t%s\n", cmplist[ASTop - A_EQ], breglist[r2]);
  fprintf(Outfile, "\tmovzbq\t%s, %s\n", breglist[r2], reglist[r2]);
  free_register(r1); */
  char buffer[32];
  snprintf(buffer, 32, "\tcmpq\t%s, %s\n", reglist[r2], reglist[r1]);
  w->writeChars(w->context, buffer);

  snprintf(buffer, 32, "\t%s\t%s\n", cmplist[ASTop - A_EQ], breglist[r2]);
  w->writeChars(w->context, buffer);

  snprintf(buffer, 32, "\tmovzbq\t%s, %s\n", breglist[r2], reglist[r2]);
  w->writeChars(w->context, buffer);

  register_free_all();
  return (r2);
}

void register_cgfuncpreamble(REGISTER_CONTEXT_PARAM, const char *funcName)
{
  /*  fprintf(Outfile,
          "\t.text\n"
          "\t.globl\t%s\n"
          "\t.type\t%s, @function\n"
          "%s:\n" "\tpushq\t%%rbp\n"
          "\tmovq\t%%rsp, %%rbp\n", name, name, name); */

  w->writeChars(w->context, "\t.text\n");

  char buffer[32];
  snprintf(buffer, 32, "\t.globl\t%s\n", funcName);
  w->writeChars(w->context, buffer);

  snprintf(buffer, 32, "\t.type\t%s, @function\n", funcName);
  w->writeChars(w->context, buffer);

  snprintf(buffer, 32, "%s:\n"
                       "\tpushq\t%%rbp\n",
           funcName);
  w->writeChars(w->context, buffer);

  w->writeChars(w->context, "\tmovq\t%%rsp, %%rbp\n");
}

void register_cgfuncpostamble(REGISTER_CONTEXT_PARAM)
{
  //fputs("\tmovl $0, %eax\n" "\tpopq     %rbp\n" "\tret\n", Outfile);
  w->writeChars(w->context, "\tmovl $0, %eax\n"
                            "\tpopq     %rbp\n"
                            "\tret\n");
}

//---------------------------------------------
// Widen the value in the register from the old
// to the new type, and return a register with
// this new value
int register_cgwiden(REGISTER_CONTEXT_PARAM, int r, int oldtype, int newtype)
{
  // Nothing to do
  return (r);
}

//========================= add in lesson 13 ----------------------

// Array of type sizes in P_XXX order.
// 0 means no size. P_NONE, P_VOID, P_CHAR, P_INT, P_LONG
static int psize[] = {0, 0, 1, 4, 8};

// Given a P_XXX type value, return the
// size of a primitive type in bytes.
int register_cgprimsize(REGISTER_CONTEXT_PARAM, int pType)
{
  // Check the type is valid
  if (pType < P_NONE || pType > P_LONG)
  {
    fprintf(stderr, "Bad type in cgprimsize()");
    exit(1);
  }
  return (psize[pType]);
}

// Print out a function preamble
void cgfuncpreamble(REGISTER_CONTEXT_PARAM, const char* name) {
  //char *name = Gsym[id].name;

  char buffer[96];
  snprintf(buffer, 96, "\t.text\n"
                       "\t.globl\t%s\n"
                       "\t.type\t%s, @function\n"
                       "%s:\n"
                       "\tpushq\t%%rbp\n"
                       "\tmovq\t%%rsp, %%rbp\n",
           name, name, name);

  w->writeChars(w->context, buffer);
}

// Generate code to return a value from a function
void register_cgreturn(REGISTER_CONTEXT_PARAM, int reg, int pType, int endlabel)
{
  char buffer[32];
  //snprintf(buffer, 32, "\t.globl\t%s\n", funcName);

  // Generate code depending on the function's type
  switch (pType)
  { // Gsym[id].type
  case P_CHAR:
    // fprintf(Outfile, "\tmovzbl\t%s, %%eax\n", breglist[reg]);
    snprintf(buffer, 32, "\tmovzbl\t%s, %%eax\n", breglist[reg]);
    break;

  case P_INT:
    //fprintf(Outfile, "\tmovl\t%s, %%eax\n", dreglist[reg]);
    snprintf(buffer, 32, "\tmovl\t%s, %%eax\n", dreglist[reg]);
    break;

  case P_LONG:
    // fprintf(Outfile, "\tmovq\t%s, %%rax\n", reglist[reg]);
    snprintf(buffer, 32, "\tmovq\t%s, %%rax\n", reglist[reg]);
    break;

  default:
    fprintf(stderr, "Bad function type in cgreturn:", pType);
    exit(1);
  }
  w->writeChars(w->context, buffer);
  register_cgjump(w, endlabel);
}

int register_cgcall(REGISTER_CONTEXT_PARAM, int r, const char *symName)
{
  // Get a new register
  int outr = register_alloc();
  /*  
  fprintf(Outfile, "\tmovq\t%s, %%rdi\n", reglist[r]);
  fprintf(Outfile, "\tcall\t%s\n", Gsym[id].name);
  fprintf(Outfile, "\tmovq\t%%rax, %s\n", reglist[outr]); */

  char buffer[32];
  snprintf(buffer, 32, "\tmovq\t%s, %%rdi\n", reglist[r]);
  w->writeChars(w->context, buffer);

  snprintf(buffer, 32, "\tcall\t%s\n", symName);
  w->writeChars(w->context, buffer);

  snprintf(buffer, 32, "\tmovq\t%%rax, %s\n", reglist[outr]);
  w->writeChars(w->context, buffer);

  free_register(r);
  return (outr);
}