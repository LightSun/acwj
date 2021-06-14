#include "register_x64.h"
#include "../ast.h"
#include "../register_pri.h"
#include "../writer.h"
// Code generator for x86-64
// Copyright (c) 2019 Warren Toomey, GPL3

// List of available registers and their names
// We need a list of byte registers, too
//static int freereg[4];
static char *reglist[REG_COUNT] = {"%r8", "%r9", "%r10", "%r11"};      //long
static char *breglist[REG_COUNT] = {"%r8b", "%r9b", "%r10b", "%r11b"}; //char
static char *dreglist[REG_COUNT] = {"%r8d", "%r9d", "%r10d", "%r11d"}; //int

// Set all registers as available
void register_x64_free_all(REGISTER_CONTEXT_PARAM)
{
  ctx->freereg[0] = ctx->freereg[1] = ctx->freereg[2] = ctx->freereg[3] = 1;
}

// Allocate a free register. Return the number of
// the register. Die if no available registers.
static int register_x64_alloc(REGISTER_CONTEXT_PARAM)
{
  for (int i = 0; i < REG_COUNT; i++)
  {
    if (ctx->freereg[i])
    {
      ctx->freereg[i] = 0;
      return (i);
    }
  }
  WRITER_PUBLISH_ERROR(REG_G_WRITER(ctx), "Out of registers!\n");
}

// Return a register to the list of available registers.
// Check to see if it's not already there.
static void register_x64_free(REGISTER_CONTEXT_PARAM, int reg)
{
  if (ctx->freereg[reg] != 0)
  {
    REG_PUBLISH_ERROR(ctx, "Error trying to free register %d\n", reg);
  }
  ctx->freereg[reg] = 1;
}

// Print out the assembly preamble
void register_x64_cgpreamble(REGISTER_CONTEXT_PARAM)
{
  register_x64_free_all(ctx);
  REG_G_WRITER(ctx)->writeChars(REG_G_WRITER_CTX(ctx), "\t.text\n");
}

// Print out the assembly postamble
void register_x64_cgpostamble(REGISTER_CONTEXT_PARAM, int sym_id)
{
}

// Load an integer literal value into a register.
// Return the number of the register.
// For x86-64, we don't need to worry about the type.
int register_x64_cgloadint(REGISTER_CONTEXT_PARAM, int value, int type)
{

  // Get a new register
  int r = register_x64_alloc(ctx);

  // Print out the code to initialise it
  //fprintf(Outfile, "\tmovq\t$%d, %s\n", value, reglist[r]);
  char buf[32];
  snprintf(buf, 32, "\tmovq\t$%d, %s\n", value, reglist[r]);
  REG_WRITE_BUF();
  return (r);
}

// Add two registers together and return
// the number of the register with the result
int register_x64_cgadd(REGISTER_CONTEXT_PARAM, int r1, int r2)
{
  //fprintf(Outfile, "\taddq\t%s, %s\n", reglist[r1], reglist[r2]);
  char buf[32];
  snprintf(buf, 32, "\taddq\t%s, %s\n", reglist[r1], reglist[r2]);
  REG_WRITE_BUF();

  register_x64_free(ctx, r1);
  return (r2);
}

// Subtract the second register from the first and
// return the number of the register with the result
int register_x64_cgsub(REGISTER_CONTEXT_PARAM, int r1, int r2)
{
  //fprintf(Outfile, "\tsubq\t%s, %s\n", reglist[r2], reglist[r1]);
  char buf[32];
  snprintf(buf, 32, "\tsubq\t%s, %s\n", reglist[r2], reglist[r1]);
  REG_WRITE_BUF();

  register_x64_free(ctx, r2);
  return (r1);
}

// Multiply two registers together and return
// the number of the register with the result
int register_x64_cgmul(REGISTER_CONTEXT_PARAM, int r1, int r2)
{
  //fprintf(Outfile, "\timulq\t%s, %s\n", reglist[r1], reglist[r2]);
  char buf[32];
  snprintf(buf, 32, "\timulq\t%s, %s\n", reglist[r1], reglist[r2]);
  REG_WRITE_BUF();

  register_x64_free(ctx, r1);
  return (r2);
}

// Divide the first register by the second and
// return the number of the register with the result
int register_x64_cgdiv(REGISTER_CONTEXT_PARAM, int r1, int r2)
{
  char buf[32];
  snprintf(buf, 32, "\tmovq\t%s,%%rax\n", reglist[r1]);
  REG_G_WRITER(ctx)->writeChars(REG_G_WRITER_CTX(ctx), buf);

  REG_G_WRITER(ctx)->writeChars(REG_G_WRITER_CTX(ctx), "\tcqo\n");

  snprintf(buf, 32, "\tidivq\t%s\n", reglist[r2]);
  REG_G_WRITER(ctx)->writeChars(REG_G_WRITER_CTX(ctx), buf);

  snprintf(buf, 32, "\tmovq\t%%rax,%s\n", reglist[r1]);
  REG_G_WRITER(ctx)->writeChars(REG_G_WRITER_CTX(ctx), buf);

  /*  fprintf(Outfile, "\tmovq\t%s,%%rax\n", reglist[r1]);
  fprintf(Outfile, "\tcqo\n");
  fprintf(Outfile, "\tidivq\t%s\n", reglist[r2]);
  fprintf(Outfile, "\tmovq\t%%rax,%s\n", reglist[r1]); */
  register_x64_free(ctx, r2);
  return (r1);
}

// Call printint() with the given register
void register_x64_cgprintint(REGISTER_CONTEXT_PARAM, int r)
{
  /* fprintf(Outfile, "\tmovq\t%s, %%rdi\n", reglist[r]);
  fprintf(Outfile, "\tcall\tprintint\n"); */

  char buf[32];
  snprintf(buf, 32, "\tmovq\t%s, %%rdi\n", reglist[r]);
  REG_G_WRITER(ctx)->writeChars(REG_G_WRITER_CTX(ctx), buf);

  REG_G_WRITER(ctx)->writeChars(REG_G_WRITER_CTX(ctx), "\tcall\tprintint\n");

  register_x64_free(ctx, r);
}

//------------- from class 6 -------------------------
int register_x64_cgloadglob(REGISTER_CONTEXT_PARAM, int sym_id, int op)
{
  // Get a new register
  int r = register_x64_alloc(ctx);
  SymTable *st = REG_G_SYM_TABLE(ctx, sym_id);

// Print out the code to initialise it
#define BUF_LEN_LOAD_GLOB 128
  char buf[BUF_LEN_LOAD_GLOB];
  switch (st->type)
  {
  case P_CHAR: //incb/decb/movzbq
  {
    switch (op)
    {
    case A_PREINC:
      snprintf(buf, BUF_LEN_LOAD_GLOB, "\tincb\t%s(\%%rip)\n", st->name);
      REG_WRITE_BUF();
      snprintf(buf, BUF_LEN_LOAD_GLOB, "\tmovzbq\t%s(%%rip), %s\n", st->name, reglist[r]);
      break;

    case A_PREDEC:
      snprintf(buf, BUF_LEN_LOAD_GLOB, "\tdecb\t%s(\%%rip)\n", st->name);
      REG_WRITE_BUF();
      snprintf(buf, BUF_LEN_LOAD_GLOB, "\tmovzbq\t%s(%%rip), %s\n", st->name, reglist[r]);
      break;
    case A_POSTINC:
      snprintf(buf, BUF_LEN_LOAD_GLOB, "\tmovzbq\t%s(%%rip), %s\n", st->name, reglist[r]);
      REG_WRITE_BUF();
      snprintf(buf, BUF_LEN_LOAD_GLOB, "\tincb\t%s(\%%rip)\n", st->name);
      break;
    case A_POSTDEC:
      snprintf(buf, BUF_LEN_LOAD_GLOB, "\tmovzbq\t%s(%%rip), %s\n", st->name, reglist[r]);
      REG_WRITE_BUF();
      snprintf(buf, BUF_LEN_LOAD_GLOB, "\tdecb\t%s(\%%rip)\n", st->name);
      break;

    default:
      return r;
    }
  }
  break;

  case P_INT: //incl/decl/movslq
    switch (op)
    {
    case A_PREINC:
      snprintf(buf, BUF_LEN_LOAD_GLOB, "\tincl\t%s(\%%rip)\n", st->name);
      REG_WRITE_BUF();
      snprintf(buf, BUF_LEN_LOAD_GLOB, "\tmovslq\t%s(%%rip), %s\n", st->name, reglist[r]);
      break;

    case A_PREDEC:
      snprintf(buf, BUF_LEN_LOAD_GLOB, "\tdecl\t%s(\%%rip)\n", st->name);
      REG_WRITE_BUF();
      snprintf(buf, BUF_LEN_LOAD_GLOB, "\tmovslq\t%s(%%rip), %s\n", st->name, reglist[r]);
      break;
    case A_POSTINC:
      snprintf(buf, BUF_LEN_LOAD_GLOB, "\tmovslq\t%s(%%rip), %s\n", st->name, reglist[r]);
      REG_WRITE_BUF();
      snprintf(buf, BUF_LEN_LOAD_GLOB, "\tincl\t%s(\%%rip)\n", st->name);
      break;
    case A_POSTDEC:
      snprintf(buf, BUF_LEN_LOAD_GLOB, "\tmovslq\t%s(%%rip), %s\n", st->name, reglist[r]);
      REG_WRITE_BUF();
      snprintf(buf, BUF_LEN_LOAD_GLOB, "\tdecl\t%s(\%%rip)\n", st->name);
      break;

    default:
      return r;
    }
    break;

  case P_LONG:
  case P_CHARPTR:
  case P_INTPTR:
  case P_LONGPTR: //incq, decq, movq
    switch (op)
    {
    case A_PREINC:
      snprintf(buf, 64, "\tincq\t%s(\%%rip)\n", st->name);
      REG_WRITE_BUF();
      snprintf(buf, 64, "\tmovq\t%s(%%rip), %s\n", st->name, reglist[r]);
      break;

    case A_PREDEC:
      snprintf(buf, 64, "\tdecq\t%s(\%%rip)\n", st->name);
      REG_WRITE_BUF();
      snprintf(buf, 64, "\tmovq\t%s(%%rip), %s\n", st->name, reglist[r]);
      break;
    case A_POSTINC:
      snprintf(buf, 64, "\tmovq\t%s(%%rip), %s\n", st->name, reglist[r]);
      REG_WRITE_BUF();
      snprintf(buf, 64, "\tincq\t%s(\%%rip)\n", st->name);
      break;
    case A_POSTDEC:
      snprintf(buf, 64, "\tmovq\t%s(%%rip), %s\n", st->name, reglist[r]);
      REG_WRITE_BUF();
      snprintf(buf, 64, "\tdecq\t%s(\%%rip)\n", st->name);
      break;

    default:
      return r;
    }
    break;

  default:
    WRITER_PUBLISH_ERROR(REG_G_WRITER(ctx), "Bad type(%d) in register_x64_cgloadglob:", st->type);
  }
  REG_WRITE_BUF();
  return (r);
}

// Store a register's value into a variable
int register_x64_cgstoreglob(REGISTER_CONTEXT_PARAM, int r, int sym_id)
{
  SymTable *st = REG_G_SYM_TABLE(ctx, sym_id);
  char buf[64];
  switch (st->type)
  {
  case P_CHAR:
    // fprintf(Outfile, "\tmovb\t%s, %s(\%%rip)\n", breglist[r], Gsym[id].name);
    snprintf(buf, 64, "\tmovb\t%s, %s(\%%rip)\n", breglist[r], st->name);
    break;

  case P_INT:
    // fprintf(Outfile, "\tmovl\t%s, %s(\%%rip)\n", dreglist[r], Gsym[id].name);
    snprintf(buf, 64, "\tmovl\t%s, %s(\%%rip)\n", dreglist[r], st->name);
    break;

  case P_LONG:
  case P_CHARPTR:
  case P_INTPTR:
  case P_LONGPTR:
    //fprintf(Outfile, "\tmovq\t%s, %s(\%%rip)\n", reglist[r], Gsym[id].name);
    snprintf(buf, 64, "\tmovq\t%s, %s(\%%rip)\n", reglist[r], st->name);
    break;

  default:
    WRITER_PUBLISH_ERROR(REG_G_WRITER(ctx), "Bad type in register_x64_cgstoreglob: %d\n", st->type);
  }
  REG_G_WRITER(ctx)->writeChars(REG_G_WRITER_CTX(ctx), buf);
  return (r);
}

// Generate a global symbol
void register_x64_cgglobsym(REGISTER_CONTEXT_PARAM, int sym_id)
{
  SymTable *st = REG_G_SYM_TABLE(ctx, sym_id);
  int typesize;
  typesize = register_x64_cgprimsize(ctx, st->type);
  //fprintf(Outfile, "\t.comm\t%s,%d,%d\n", Gsym[id].name, typesize, typesize);

  REG_WRITE_FMT_BUF_32("\t.data\n"
                       "\t.globl\t%s\n",
                       st->name);

  // Generate the space
  for (size_t i = 0; i < st->size; i++)
  {
    switch (typesize)
    {
    case 1:
      REG_WRITE_FMT_BUF_32("%s:\t.byte\t0\n", st->name);
      break;
    case 4:
      REG_WRITE_FMT_BUF_32("%s:\t.long\t0\n", st->name);
      break;
    case 8:
      REG_WRITE_FMT_BUF_32("%s:\t.quad\t0\n", st->name);
      break;
    default:
      REG_PUBLISH_ERROR(ctx, "Unknown typesize in register_x64_cgglobsym(): %d", typesize);
    }
  }
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

  char buf[32];
  snprintf(buf, 32, "\tcmpq\t%s, %s\n", reglist[r2], reglist[r1]);
  REG_WRITE_BUF();

  snprintf(buf, 32, "\t%s\t%s\n", how, breglist[r2]);
  REG_WRITE_BUF();

  snprintf(buf, 32, "\tandq\t$255,%s\n", reglist[r2]);
  REG_WRITE_BUF();

  register_x64_free(ctx, r1);
  return (r2);
}

int register_x64_cgequal(REGISTER_CONTEXT_PARAM, int r1, int r2)
{
  return (cgcompare(ctx, r1, r2, "sete"));
}
int register_x64_cgnotequal(REGISTER_CONTEXT_PARAM, int r1, int r2)
{
  return (cgcompare(ctx, r1, r2, "setne"));
}
int register_x64_cglessthan(REGISTER_CONTEXT_PARAM, int r1, int r2)
{
  return (cgcompare(ctx, r1, r2, "setl"));
}
int register_x64_cggreaterthan(REGISTER_CONTEXT_PARAM, int r1, int r2)
{
  return (cgcompare(ctx, r1, r2, "setg"));
}
int register_x64_cglessequal(REGISTER_CONTEXT_PARAM, int r1, int r2)
{
  return (cgcompare(ctx, r1, r2, "setle"));
}
int register_x64_cggreaterequal(REGISTER_CONTEXT_PARAM, int r1, int r2)
{
  return (cgcompare(ctx, r1, r2, "setge"));
}

//------------------------- if statements ---------------------------

void register_x64_cgjump(REGISTER_CONTEXT_PARAM, int l)
{
  // fprintf(Outfile, "\tjmp\tL%d\n", l);
  char buf[32];
  snprintf(buf, 32, "\tjmp\tL%d\n", l);
  REG_G_WRITER(ctx)->writeChars(REG_G_WRITER_CTX(ctx), buf);
}

void register_x64_cglabel(REGISTER_CONTEXT_PARAM, int l)
{
  // fprintf(Outfile, "L%d:\n", l);
  char buf[32];
  snprintf(buf, 32, "L%d:\n", l);
  REG_G_WRITER(ctx)->writeChars(REG_G_WRITER_CTX(ctx), buf);
}

// List of inverted jump instructions,
// = -> !=, != -> =, < -> >=, > -> <=, <= -> >, >= -> <
// in AST order: A_EQ, A_NE, A_LT, A_GT, A_LE, A_GE
static char *invcmplist[] = {"jne", "je", "jge", "jle", "jg", "jl"};

// Compare two registers and jump if false.
int register_x64_cgcompare_and_jump(REGISTER_CONTEXT_PARAM, int ASTop, int r1, int r2, int label)
{

  // Check the range of the AST operation
  if (ASTop < A_EQ || ASTop > A_GE)
  {
    REG_PUBLISH_ERROR(ctx, "Bad ASTop in register_x64_cgcompare_and_jump()");
  }

  /*  fprintf(Outfile, "\tcmpq\t%s, %s\n", reglist[r2], reglist[r1]);
  fprintf(Outfile, "\t%s\tL%d\n", invcmplist[ASTop - A_EQ], label);
  freeall_registers(); */

  char buf[32];
  snprintf(buf, 32, "\tcmpq\t%s, %s\n", reglist[r2], reglist[r1]);
  REG_G_WRITER(ctx)->writeChars(REG_G_WRITER_CTX(ctx), buf);

  snprintf(buf, 32, "\t%s\tL%d\n", invcmplist[ASTop - A_EQ], label);
  REG_G_WRITER(ctx)->writeChars(REG_G_WRITER_CTX(ctx), buf);

  register_x64_free_all(ctx);
  return (NOREG);
}

// List of comparison instructions,
// in AST order: A_EQ, A_NE, A_LT, A_GT, A_LE, A_GE
static char *cmplist[] =
    {"sete", "setne", "setl", "setg", "setle", "setge"};

// Compare two registers and set if true.
int register_x64_cgcompare_and_set(REGISTER_CONTEXT_PARAM, int ASTop, int r1, int r2)
{

  // Check the range of the AST operation
  if (ASTop < A_EQ || ASTop > A_GE)
  {
    // WRITER_PUBLISH_ERROR(REG_G_WRITER(ctx), "Bad ASTop in cgcompare_and_set()");
    REG_PUBLISH_ERROR(ctx, "Bad ASTop in cgcompare_and_set()");
  }

  /* fprintf(Outfile, "\tcmpq\t%s, %s\n", reglist[r2], reglist[r1]);
  fprintf(Outfile, "\t%s\t%s\n", cmplist[ASTop - A_EQ], breglist[r2]);
  fprintf(Outfile, "\tmovzbq\t%s, %s\n", breglist[r2], reglist[r2]);
  free_register(r1); */
  char buf[32];
  snprintf(buf, 32, "\tcmpq\t%s, %s\n", reglist[r2], reglist[r1]);
  REG_G_WRITER(ctx)->writeChars(REG_G_WRITER_CTX(ctx), buf);

  snprintf(buf, 32, "\t%s\t%s\n", cmplist[ASTop - A_EQ], breglist[r2]);
  REG_G_WRITER(ctx)->writeChars(REG_G_WRITER_CTX(ctx), buf);

  snprintf(buf, 32, "\tmovzbq\t%s, %s\n", breglist[r2], reglist[r2]);
  REG_G_WRITER(ctx)->writeChars(REG_G_WRITER_CTX(ctx), buf);

  register_x64_free_all(ctx);
  return (r2);
}

void register_x64_cgfuncpreamble(REGISTER_CONTEXT_PARAM, int sym_id)
{
  /*  fprintf(Outfile,
          "\t.text\n"
          "\t.globl\t%s\n"
          "\t.type\t%s, @function\n"
          "%s:\n" "\tpushq\t%%rbp\n"
          "\tmovq\t%%rsp, %%rbp\n", name, name, name); */

  SymTable *st = REG_G_SYM_TABLE(ctx, sym_id);
  char buf[96];
  snprintf(buf, 96, "\t.text\n"
                    "\t.globl\t%s\n"
                    "\t.type\t%s, @function\n"
                    "%s:\n"
                    "\tpushq\t%%rbp\n"
                    "\tmovq\t%%rsp, %%rbp\n",
           st->name, st->name, st->name);

  REG_G_WRITER(ctx)->writeChars(REG_G_WRITER_CTX(ctx), buf);
}

void register_x64_cgfuncpostamble(REGISTER_CONTEXT_PARAM, int sym_id)
{
  //cglabel(Gsym[id].endlabel);
  //fputs("\tpopq	%rbp\n" "\tret\n", Outfile);
  register_x64_cglabel(ctx, SYM_END_LABEL(REG_G_GLOBAL_STATE(ctx), sym_id));
  REG_G_WRITER(ctx)->writeChars(REG_G_WRITER_CTX(ctx), "\tpopq     %rbp\n"
                                                       "\tret\n");
}

//---------------------------------------------
// Widen the value in the register from the old
// to the new type, and return a register with
// this new value
int register_x64_cgwiden(REGISTER_CONTEXT_PARAM, int r, int oldtype, int newtype)
{
  // Nothing to do
  return (r);
}

//========================= add in lesson 13 ----------------------

// Array of type sizes in P_XXX order.
// 0 means no size. P_NONE, P_VOID, P_CHAR, P_INT, P_LONG, PTR(3)
static int psize[] = {0, 0, 1, 4, 8, 8, 8, 8};

// Given a P_XXX type value, return the
// size of a primitive type in bytes.
int register_x64_cgprimsize(REGISTER_CONTEXT_PARAM, int pType)
{
  // Check the type is valid
  if (pType < P_NONE || pType > P_LONGPTR)
  {
    REG_PUBLISH_ERROR(ctx, "Bad type(%d) in cgprimsize()", pType);
  }
  return (psize[pType]);
}

// Generate code to return a value from a function
void register_x64_cgreturn(REGISTER_CONTEXT_PARAM, int reg, int sym_id)
{
  SymTable *st = REG_G_SYM_TABLE(ctx, sym_id);
  char buf[32];
  //snprintf(buf, 32, "\t.globl\t%s\n", funcName);

  // Generate code depending on the function's type
  switch (st->type)
  { // Gsym[id].type
  case P_CHAR:
    // fprintf(Outfile, "\tmovzbl\t%s, %%eax\n", breglist[reg]);
    snprintf(buf, 32, "\tmovzbl\t%s, %%eax\n", breglist[reg]);
    break;

  case P_INT:
    //fprintf(Outfile, "\tmovl\t%s, %%eax\n", dreglist[reg]);
    snprintf(buf, 32, "\tmovl\t%s, %%eax\n", dreglist[reg]);
    break;

  case P_LONG:
    // fprintf(Outfile, "\tmovq\t%s, %%rax\n", reglist[reg]);
    snprintf(buf, 32, "\tmovq\t%s, %%rax\n", reglist[reg]);
    break;

  default:
    REG_PUBLISH_ERROR(ctx, "Bad function type(%d) in cgreturn:", st->type);
  }
  REG_G_WRITER(ctx)->writeChars(REG_G_WRITER_CTX(ctx), buf);
  register_x64_cgjump(ctx, st->endlabel);
}

int register_x64_cgcall(REGISTER_CONTEXT_PARAM, int r, int sym_id)
{
  // Get a new register
  int outr = register_x64_alloc(ctx);
  /*  
  fprintf(Outfile, "\tmovq\t%s, %%rdi\n", reglist[r]);
  fprintf(Outfile, "\tcall\t%s\n", Gsym[id].name);
  fprintf(Outfile, "\tmovq\t%%rax, %s\n", reglist[outr]); */

  char buf[32];
  snprintf(buf, 32, "\tmovq\t%s, %%rdi\n", reglist[r]);
  REG_G_WRITER(ctx)->writeChars(REG_G_WRITER_CTX(ctx), buf);

  snprintf(buf, 32, "\tcall\t%s\n", REG_G_SYM_TABLE(ctx, sym_id)->name);
  REG_G_WRITER(ctx)->writeChars(REG_G_WRITER_CTX(ctx), buf);

  snprintf(buf, 32, "\tmovq\t%%rax, %s\n", reglist[outr]);
  REG_G_WRITER(ctx)->writeChars(REG_G_WRITER_CTX(ctx), buf);

  register_x64_free(ctx, r);
  return (outr);
}

//------------------------ get addr/de-ref-addr ------------

// Generate code to load the address of a global
// identifier into a variable. Return a new register
int register_x64_cgaddress(REGISTER_CONTEXT_PARAM, int id)
{
  int r = register_x64_alloc(ctx);
  //REG_G_SYM_TABLE(ctx, id)->name

  //fprintf(Outfile, "\tleaq\t%s(%%rip), %s\n", Gsym[id].name, reglist[r]);
  char buf[48];
  snprintf(buf, 48, "\tleaq\t%s(%%rip), %s\n", REG_G_SYM_TABLE(ctx, id)->name, reglist[r]);
  //The leaq instruction loads the address of the named identifie

  REG_WRITE_BUF();
  return (r);
}
// Dereference a pointer to get the value it
// pointing at into the same register
int register_x64_cgderef(REGISTER_CONTEXT_PARAM, int r, int pType)
{
  switch (pType)
  {
  case P_CHARPTR:
  {
    //fprintf(Outfile, "\tmovzbq\t(%s), %s\n", reglist[r], reglist[r]);
    char buf[32];
    snprintf(buf, 32, "\tmovzbq\t(%s), %s\n", reglist[r], reglist[r]);
    REG_WRITE_BUF();
    break;
  }
  case P_INTPTR:
  case P_LONGPTR:
  {
    //fprintf(Outfile, "\tmovq\t(%s), %s\n", reglist[r], reglist[r]);
    char buf[32];
    snprintf(buf, 32, "\tmovq\t(%s), %s\n", reglist[r], reglist[r]);
    REG_WRITE_BUF();
    break;
  }
  }
  return (r);
}

// Shift a register left by a constant. '<<'
int register_x64_cgshlconst(REGISTER_CONTEXT_PARAM, int r, int val)
{
  //fprintf(Outfile, "\tsalq\t$%d, %s\n", val, reglist[r]);
  char buf[32];
  snprintf(buf, 32, "\tsalq\t$%d, %s\n", val, reglist[r]);
  REG_WRITE_BUF();
  return (r);
}

// a = *b
int register_x64_cgstorederef(REGISTER_CONTEXT_PARAM, int r1, int r2, int type)
{

  char buf[32];
  switch (type)
  {
  case P_CHAR:
    //fprintf(Outfile, "\tmovb\t%s, (%s)\n", breglist[r1], reglist[r2]);
    snprintf(buf, 32, "\tmovb\t%s, (%s)\n", breglist[r1], reglist[r2]);
    REG_WRITE_BUF();
    break;

  case P_INT:
    // fprintf(Outfile, "\tmovq\t%s, (%s)\n", reglist[r1], reglist[r2]);
    snprintf(buf, 32, "\tmovq\t%s, (%s)\n", reglist[r1], reglist[r2]);
    REG_WRITE_BUF();
    break;

  case P_LONG:
    // fprintf(Outfile, "\tmovq\t%s, (%s)\n", reglist[r1], reglist[r2]);
    snprintf(buf, 32, "\tmovq\t%s, (%s)\n", reglist[r1], reglist[r2]);
    REG_WRITE_BUF();
    break;

  default:
    REG_PUBLISH_ERROR(ctx, "Can't cgstorederef on type = %d", type);
  }
  return (r1);
}

//gen label for store string.
void register_x64_cgglobstr(REGISTER_CONTEXT_PARAM, int l, const char *strvalue)
{
  char *cptr;
  register_x64_cglabel(ctx, l);
  char buf[32];
  for (cptr = strvalue; *cptr; cptr++)
  {
    //fprintf(Outfile, "\t.byte\t%d\n", *cptr);
    snprintf(buf, 32, "\t.byte\t%d\n", *cptr);
    REG_WRITE_BUF();
  }
  //fprintf(Outfile, "\t.byte\t0\n");
  REG_WRITE_STR("\t.byte\t0\n");
}

//load the string first addr to register,
int register_x64_cgloadglobstr(REGISTER_CONTEXT_PARAM, int id)
{
  // Get a new register
  int r = register_x64_alloc(ctx);
  //fprintf(Outfile, "\tleaq\tL%d(\%%rip), %s\n", id, reglist[r]);
  char buf[32];
  snprintf(buf, 32, "\tleaq\tL%d(\%%rip), %s\n", id, reglist[r]);
  REG_WRITE_BUF();
  return (r);
}

//----------------------------
int register_x64_cgand(REGISTER_CONTEXT_PARAM, int r1, int r2)
{
  char buf[32];
  snprintf(buf, 32, "\tandq\t%s, %s\n", reglist[r1], reglist[r2]);
  REG_WRITE_BUF();
  register_x64_free(ctx, r1);
  return (r2);
}

int register_x64_cgor(REGISTER_CONTEXT_PARAM, int r1, int r2)
{
  char buf[32];
  snprintf(buf, 32, "\torq\t%s, %s\n", reglist[r1], reglist[r2]);
  REG_WRITE_BUF();
  register_x64_free(ctx, r1);
  return (r2);
}
int register_x64_cgxor(REGISTER_CONTEXT_PARAM, int r1, int r2)
{
  char buf[32];
  snprintf(buf, 32, "\txorq\t%s, %s\n", reglist[r1], reglist[r2]);
  REG_WRITE_BUF();
  register_x64_free(ctx, r1);
  return (r2);
}
int register_x64_cgshl(REGISTER_CONTEXT_PARAM, int r1, int r2)
{
  char buf[32];
  snprintf(buf, 32, "\tmovb\t%s, %%cl\n", breglist[r2]);
  REG_WRITE_BUF();
  snprintf(buf, 32, "\tshlq\t%%cl, %s\n", reglist[r1]);
  REG_WRITE_BUF();

  register_x64_free(ctx, r1);
  return (r2);
}

int register_x64_cgshr(REGISTER_CONTEXT_PARAM, int r1, int r2)
{
  char buf[32];
  snprintf(buf, 32, "\tmovb\t%s, %%cl\n", breglist[r2]);
  REG_WRITE_BUF();
  snprintf(buf, 32, "\tshrq\t%%cl, %s\n", reglist[r1]);
  REG_WRITE_BUF();

  register_x64_free(ctx, r1);
  return (r2);
}

// Negate a register's value
int register_x64_cgnegate(REGISTER_CONTEXT_PARAM, int r)
{
  // fprintf(Outfile, "\tnegq\t%s\n", reglist[r]); return (r);
  char buf[32];
  snprintf(buf, 32, "\tnegq\t%s\n", reglist[r]);
  REG_WRITE_BUF();
  return r;
}

// Invert a register's value
int register_x64_cginvert(REGISTER_CONTEXT_PARAM, int r)
{
  char buf[32];
  snprintf(buf, 32, "\tnotq\t%s\n", reglist[r]);
  REG_WRITE_BUF();
  return r;
}
// Logically negate a register's value
int register_x64_cglognot(REGISTER_CONTEXT_PARAM, int r)
{
  // fprintf(Outfile, "\ttest\t%s, %s\n", reglist[r], reglist[r]);
  // fprintf(Outfile, "\tsete\t%s\n", breglist[r]);
  // fprintf(Outfile, "\tmovzbq\t%s, %s\n", breglist[r], reglist[r]);
  char buf[32];
  snprintf(buf, 32, "\ttest\t%s, %s\n", reglist[r], reglist[r]);
  REG_WRITE_BUF();
  snprintf(buf, 32, "\tsete\t%s\n", breglist[r]);
  REG_WRITE_BUF();
  snprintf(buf, 32, "\tnegq\t%s\n", reglist[r]);
  REG_WRITE_BUF();
  return (r);
}

int register_x64_cgboolean(REGISTER_CONTEXT_PARAM, int r, int op, int label)
{
  /*  fprintf(Outfile, "\ttest\t%s, %s\n", reglist[r], reglist[r]);
  if (op == A_IF || op == A_WHILE)
    fprintf(Outfile, "\tje\tL%d\n", label);
  else {
    fprintf(Outfile, "\tsetnz\t%s\n", breglist[r]);
    fprintf(Outfile, "\tmovzbq\t%s, %s\n", breglist[r], reglist[r]);
  } */
  char buf[32];
  snprintf(buf, 32, "\ttest\t%s, %s\n", reglist[r], reglist[r]);
  REG_WRITE_BUF();
  if (op == A_IF || op == A_WHILE)
  {
    snprintf(buf, 32, "\tje\tL%d\n", label);
    REG_WRITE_BUF();
  }
  else
  {
    snprintf(buf, 32, "\tsetnz\t%s\n", breglist[r]);
    REG_WRITE_BUF();
    snprintf(buf, 32, "\tmovzbq\t%s, %s\n", breglist[r], reglist[r]);
    REG_WRITE_BUF();
  }

  return (r);
}