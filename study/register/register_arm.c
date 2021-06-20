#include "register_arm.h"
#include "../ast.h"
#include "../sym.h"
#include "../utils.h"
#include "../writer.h"

// Allocate a free register. Return the number of
// the register. Die if no available registers.
static int register_arm_alloc(REGISTER_CONTEXT_PARAM)
{
    for (int i = 0; i < REG_COUNT; i++)
    {
        if (ctx->freereg[i])
        {
            ctx->freereg[i] = 0;
            return (i);
        }
    }
    REG_PUBLISH_ERROR(ctx, "Out of registers");
    return (NOREG); // Keep -Wall happy
}
// Return a register to the list of available registers.
// Check to see if it's not already there.
static void register_arm_free(REGISTER_CONTEXT_PARAM, int reg)
{
    if (ctx->freereg[reg] != 0)
    {
        REG_PUBLISH_ERROR(ctx, "Error trying to free register = %d", reg);
    }
    ctx->freereg[reg] = 1;
}
// Determine the offset of a large integer
// literal from the .L3 label. If the integer
// isn't in the list, add it.
static void set_int_offset(REGISTER_CONTEXT_PARAM, int val)
{
    int offset = -1;

    // See if it is already there
    for (int i = 0; i < ctx->intslot; i++)
    {
        if (ctx->intlist[i] == val)
        {
            offset = 4 * i;
            break;
        }
    }

    // Not in the list, so add it
    if (offset == -1)
    {
        offset = 4 * ctx->intslot;
        if (ctx->intslot == REG_MAXINTS)
        {
            REG_PUBLISH_ERROR(ctx, "Out of int slots in set_int_offset()");
        }
        ctx->intlist[ctx->intslot++] = val;
    }
    // Load r3 with this offset
    //fprintf(Outfile, "\tldr\tr3, .L3+%d\n", offset);
    char buf[32];
    snprintf(buf, 32, "\tldr\tr3, .L3+%d\n", offset);
    REG_WRITE_BUF();
}

// Determine the offset of a variable from the .L2
// label. Yes, this is inefficient code.
static void set_var_offset(REGISTER_CONTEXT_PARAM, int id)
{
    struct GlobalState *gs = REG_G_GLOBAL_STATE(ctx);
    int offset = 0;
    // Walk the symbol table up to id.
    // Find S_VARIABLEs and add on 4 until
    // we get to our variable
    for (int i = 0; i < id; i++)
    {
        if (gs->syms[i].stype == S_VARIABLE)
            offset += 4;
    }
    // Load r3 with this offset
    // fprintf(Outfile, "\tldr\tr3, .L2+%d\n", offset);
    char buf[32];
    snprintf(buf, 32, "\tldr\tr3, .L2+%d\n", offset);
    REG_WRITE_BUF();
}

//------------------------------------------------------------------
// List of available registers and their names.
//static int freereg[4]; //move to context
static char *reglist[REG_COUNT] = {"r4", "r5", "r6", "r7"};

void register_arm_free_all(REGISTER_CONTEXT_PARAM)
{
    ctx->freereg[0] = ctx->freereg[1] = ctx->freereg[2] = ctx->freereg[3] = 1;
}

// Print out the assembly preamble
void register_arm_cgpreamble(REGISTER_CONTEXT_PARAM)
{
    register_arm_free_all(ctx);
    REG_G_WRITER(ctx)->writeChars(REG_G_WRITER_CTX(ctx), "\t.text\n");
    // fputs("\t.text\n", Outfile);
}

void register_arm_cgpostamble(REGISTER_CONTEXT_PARAM, int sym_id)
{
    struct GlobalState *gs = REG_G_GLOBAL_STATE(ctx);
    // Print out the global variables
    REG_G_WRITER(ctx)->writeChars(REG_G_WRITER_CTX(ctx), ".L2:\n");
    char buf[32];
    for (int i = 0; i < gs->globs; i++)
    {
        if (gs->syms[i].stype == S_VARIABLE)
        {
            snprintf(buf, 32, "\t.word %s\n", gs->syms[i].name);
            REG_WRITE_BUF();
        }
    }
    // Print out the integer literals
    REG_G_WRITER(ctx)->writeChars(REG_G_WRITER_CTX(ctx), ".L3:\n");
    for (int i = 0; i < ctx->intslot; i++)
    {
        // fprintf(Outfile, "\t.word %d\n", Intlist[i]);
        snprintf(buf, 32, "\t.word %d\n", ctx->intlist[i]);
        REG_WRITE_BUF();
    }
}

int register_arm_cgloadint(REGISTER_CONTEXT_PARAM, int value, int type)
{
    // Get a new register
    int r = register_arm_alloc(ctx);

    char buf[32];
    // If the literal value is small, do it with one instruction
    if (value <= 1000)
        snprintf(buf, 32, "\tmov\t%s, #%d\n", reglist[r], value);
    else
    {
        set_int_offset(ctx, value);
        snprintf(buf, 32, "\tldr\t%s, [r3]\n", reglist[r]);
    }
    REG_WRITE_BUF();

    return (r);
}
int register_arm_cgadd(REGISTER_CONTEXT_PARAM, int r1, int r2)
{
    //fprintf(Outfile, "\tadd\t%s, %s, %s\n", reglist[r2], reglist[r1], reglist[r2]);
    char buf[32];
    snprintf(buf, 32, "\tadd\t%s, %s, %s\n", reglist[r2], reglist[r1], reglist[r2]);
    REG_WRITE_BUF();
    register_arm_free(ctx, r1);
    return (r2);
}
int register_arm_cgsub(REGISTER_CONTEXT_PARAM, int r1, int r2)
{
    //fprintf(Outfile, "\tsub\t%s, %s, %s\n", reglist[r1], reglist[r1], reglist[r2]);
    char buf[32];
    snprintf(buf, 32, "\tsub\t%s, %s, %s\n", reglist[r1], reglist[r1], reglist[r2]);
    REG_WRITE_BUF();
    register_arm_free(ctx, r2);
    return (r1);
}
int register_arm_cgmul(REGISTER_CONTEXT_PARAM, int r1, int r2)
{
    char buf[32];
    snprintf(buf, 32, "\tmul\t%s, %s, %s\n", reglist[r2], reglist[r1], reglist[r2]);
    REG_WRITE_BUF();
    register_arm_free(ctx, r1);
    return (r2);
}
int register_arm_cgdiv(REGISTER_CONTEXT_PARAM, int r1, int r2)
{
    // To do a divide: r0 holds the dividend, r1 holds the divisor.
    // The quotient is in r0.
    /*   fprintf(Outfile, "\tmov\tr0, %s\n", reglist[r1]);
  fprintf(Outfile, "\tmov\tr1, %s\n", reglist[r2]);
  fprintf(Outfile, "\tbl\t__aeabi_idiv\n");
  fprintf(Outfile, "\tmov\t%s, r0\n", reglist[r1]);
  free_register(r2); */
    char buf[32];
    snprintf(buf, 32, "\tmov\tr0, %s\n", reglist[r1]);
    REG_WRITE_BUF();

    snprintf(buf, 32, "\tmov\tr1, %s\n", reglist[r2]);
    REG_WRITE_BUF();

    REG_WRITE_STR("\tbl\t__aeabi_idiv\n");

    snprintf(buf, 32, "\tmov\t%s, r0\n", reglist[r1]);
    REG_WRITE_BUF();

    return (r1);
}

void register_arm_cgprintint(REGISTER_CONTEXT_PARAM, int r)
{
    /*   fprintf(Outfile, "\tmov\tr0, %s\n", reglist[r]);
    fprintf(Outfile, "\tbl\tprintint\n");
    fprintf(Outfile, "\tnop\n");
    free_register(r); */
    char buf[32];
    snprintf(buf, 32, "\tmov\tr0, %s\n", reglist[r]);
    REG_WRITE_BUF();

    REG_WRITE_STR("\tbl\tprintint\n");
    REG_WRITE_STR("\tnop\n");
    register_arm_free(ctx, r);
}

// Load a value from a variable into a register.
// Return the number of the register
//name: identifier name
int register_arm_cgloadglob(REGISTER_CONTEXT_PARAM, int sym_id, int op)
{
    int r = register_arm_alloc(ctx);
    //TODO Get the offset to the variable
    set_var_offset(ctx, sym_id);

    char buf[32];
    snprintf(buf, 32, "\tldr\t%s, [r3]\n", reglist[r]);
    REG_WRITE_BUF();
    //fprintf(Outfile, "\tldr\t%s, [r3]\n", reglist[r]);
    return (r);
}
// Store a register's value into a variable
//name: identifier name
int register_arm_cgstoreglob(REGISTER_CONTEXT_PARAM, int r, int sym_id)
{
    // Get the offset to the variable
    set_var_offset(ctx, sym_id);
    SymTable *st = REG_G_SYM_TABLE(ctx, sym_id);

    char buf[32];
    switch (st->type)
    {
    case P_CHAR:
        //fprintf(Outfile, "\tstrb\t%s, [r3]\n", reglist[r]);
        snprintf(buf, 32, "\tstrb\t%s, [r3]\n", reglist[r]);
        break;
    case P_INT:
    case P_LONG:
        //fprintf(Outfile, "\tstr\t%s, [r3]\n", reglist[r]);
        snprintf(buf, 32, "\tstr\t%s, [r3]\n", reglist[r]);
        break;
    default:
        REG_PUBLISH_ERROR(ctx, "Bad type in cgloadglob: %d", st->type);
    }
    REG_WRITE_BUF();
    return (r);
}
// Generate a global symbol
//pType: one of Primitive types
//name: identifier name
void register_arm_cgglobsym(REGISTER_CONTEXT_PARAM, int sym_id)
{
    SymTable *st = REG_G_SYM_TABLE(ctx, sym_id);
    int typesize;
    // Get the size of the type
    typesize = register_arm_cgprimsize(ctx, st->type);

    //fprintf(Outfile, "\t.comm\t%s,%d,%d\n", Gsym[id].name, typesize, typesize);
    REG_WRITE_FMT_BUF_32("\t.data\n"
                         "\t.globl\t%s\n",
                         st->name);
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
        default:
            REG_PUBLISH_ERROR(ctx, "Unknown typesize in register_arm_cgglobsym(): %d", typesize);
        }
    }
}

//---------------- if statement ----------------
// Generate a jump to a label
void register_arm_cgjump(REGISTER_CONTEXT_PARAM, int l)
{
    char buf[32];
    snprintf(buf, 32, "\tb\tL%d\n", l);
    REG_WRITE_BUF();
}

// Generate a label
void register_arm_cglabel(REGISTER_CONTEXT_PARAM, int l)
{
    // fprintf(Outfile, "L%d:\n", l);
    char buf[32];
    snprintf(buf, 32, "L%d:\n", l);
    REG_WRITE_BUF();
}

// List of inverted branch instructions,
// in AST order: A_EQ, A_NE, A_LT, A_GT, A_LE, A_GE
static char *brlist[] = {"bne", "beq", "bge", "ble", "bgt", "blt"};
int register_arm_cgcompare_and_jump(REGISTER_CONTEXT_PARAM, int asTop, int r1, int r2, int label)
{
    // Check the range of the AST operation
    if (asTop < A_EQ || asTop > A_GE)
    {
        REG_PUBLISH_ERROR(ctx, "Bad ASTop in cgcompare_and_set()");
    }
    char buf[32];
    snprintf(buf, 32, "\tcmp\t%s, %s\n", reglist[r1], reglist[r2]);
    REG_WRITE_BUF();

    snprintf(buf, 32, "\t%s\tL%d\n", brlist[asTop - A_EQ], label);
    REG_WRITE_BUF();

    register_arm_free_all(ctx);
    /*  fprintf(Outfile, "\tcmp\t%s, %s\n", reglist[r1], reglist[r2]);
  fprintf(Outfile, "\t%s\tL%d\n", brlist[asTop - A_EQ], label);
  freeall_registers(); */
    return (NOREG);
}

// List of comparison instructions,
// in AST order: A_EQ, A_NE, A_LT, A_GT, A_LE, A_GE
static char *cmplist[] =
    {"moveq", "movne", "movlt", "movgt", "movle", "movge"};

// List of inverted jump instructions,
// in AST order: A_EQ, A_NE, A_LT, A_GT, A_LE, A_GE
static char *invcmplist[] =
    {"movne", "moveq", "movge", "movle", "movgt", "movlt"};
// Compare two registers and set if true.
int register_arm_cgcompare_and_set(REGISTER_CONTEXT_PARAM, int asTop, int r1, int r2)
{

    // Check the range of the AST operation
    if (asTop < A_EQ || asTop > A_GE)
    {
        REG_PUBLISH_ERROR(ctx, "Bad ASTop(%d) in cgcompare_and_set()", asTop);
    }
    char buf[32];
    snprintf(buf, 32, "\tcmp\t%s, %s\n", reglist[r1], reglist[r2]);
    REG_WRITE_BUF();

    snprintf(buf, 32, "\t%s\t%s, #1\n", cmplist[asTop - A_EQ], reglist[r2]);
    REG_WRITE_BUF();

    snprintf(buf, 32, "\t%s\t%s, #0\n", invcmplist[asTop - A_EQ], reglist[r2]);
    REG_WRITE_BUF();

    snprintf(buf, 32, "\tuxtb\t%s, %s\n", reglist[r2], reglist[r2]);
    REG_WRITE_BUF();

    register_arm_free(ctx, r1);
    /* fprintf(Outfile, "\tcmp\t%s, %s\n", reglist[r1], reglist[r2]);
  fprintf(Outfile, "\t%s\t%s, #1\n", cmplist[ASTop - A_EQ], reglist[r2]);
  fprintf(Outfile, "\t%s\t%s, #0\n", invcmplist[ASTop - A_EQ], reglist[r2]);
  fprintf(Outfile, "\tuxtb\t%s, %s\n", reglist[r2], reglist[r2]);
  free_register(r1); */
    return (r2);
}

//-------------- function -----------------------
void register_arm_cgfuncpreamble(REGISTER_CONTEXT_PARAM, int sym_id)
{
    SymTable *st = REG_G_SYM_TABLE(ctx, sym_id);
    //  char *name = Gsym[id].name;
    char buf[128];

    snprintf(buf, 128,
             "\t.text\n"
             "\t.globl\t%s\n"
             "\t.type\t%s, \%%function\n"
             "%s:\n"
             "\tpush\t{fp, lr}\n"
             "\tadd\tfp, sp, #4\n"
             "\tsub\tsp, sp, #8\n"
             "\tstr\tr0, [fp, #-8]\n",
             st->name, st->name, st->name);
    REG_WRITE_BUF();
}

void register_arm_cgfuncpostamble(REGISTER_CONTEXT_PARAM, int sym_id)
{
    register_arm_cglabel(ctx, REG_G_SYM_TABLE(ctx, sym_id)->endlabel);
    // fputs("\tsub\tsp, fp, #4\n" "\tpop\t{fp, pc}\n" "\t.align\t2\n", Outfile);
    REG_G_WRITER(ctx)->writeChars(REG_G_WRITER_CTX(ctx), "\tsub\tsp, fp, #4\n"
                                                         "\tpop\t{fp, pc}\n"
                                                         "\t.align\t2\n");
}

//---------------- widen --------------------------
int register_arm_cgwiden(REGISTER_CONTEXT_PARAM, int r, int oldtype, int newtype)
{
    return r;
}

// Array of type sizes in P_XXX order.
// 0 means no size.
static int psize[] = {0, 0, 1, 4, 4};
//get the data size of the primitive type.
int register_arm_cgprimsize(REGISTER_CONTEXT_PARAM, int pType)
{
    // Check the type is valid
    if (pType < P_NONE || pType > P_LONGPTR)
    {
        REG_PUBLISH_ERROR(ctx, "Bad type in cgprimsize()");
    }
    return (psize[pType]);
}

/**
 * To call a function with one argument, we need to copy the register with the argument value into %rdi.
 *  On return, we need to copy the returned value from %rax into the register that will have this new value:
 * */
int register_arm_cgcall(REGISTER_CONTEXT_PARAM, int r, int sym_id)
{
    /*   fprintf(Outfile, "\tmov\tr0, %s\n", reglist[r]);
  fprintf(Outfile, "\tbl\t%s\n", Gsym[id].name);
  fprintf(Outfile, "\tmov\t%s, r0\n", reglist[r]);
  return (r); */
    char buf[32];
    snprintf(buf, 32, "\tmov\tr0, %s\n", reglist[r]);
    REG_WRITE_BUF();

    snprintf(buf, 32, "\tbl\t%s\n", REG_G_SYM_TABLE(ctx, sym_id)->name);
    REG_WRITE_BUF();

    snprintf(buf, 32, "\tmov\t%s, r0\n", reglist[r]);
    REG_WRITE_BUF();
    return r;
}

//return
void register_arm_cgreturn(REGISTER_CONTEXT_PARAM, int reg, int sym_id)
{
    /*  fprintf(Outfile, "\tmov\tr0, %s\n", reglist[reg]);
    cgjump(Gsym[id].endlabel); */

    char buf[32];
    snprintf(buf, 32, "\tmov\tr0, %s\n", reglist[reg]);
    REG_WRITE_BUF();

    register_arm_cgjump(ctx, REG_G_SYM_TABLE(ctx, sym_id)->endlabel);
}

//---------------------- pointer ---------------------
//get addr
int register_arm_cgaddress(REGISTER_CONTEXT_PARAM, int id)
{
    // Get a new register
    int r = register_arm_alloc(ctx);

    // Get the offset to the variable
    set_var_offset(ctx, id);
    //fprintf(Outfile, "\tmov\t%s, r3\n", reglist[r]);
    REG_WRITE_FMT_BUF_32("\tmov\t%s, r3\n", reglist[r]);
    return (r);
}
// de - ref-addr
int register_arm_cgderef(REGISTER_CONTEXT_PARAM, int r, int pType)
{
    switch (pType)
    {
    case P_CHARPTR:
        //fprintf(Outfile, "\tldrb\t%s, [%s]\n", reglist[r], reglist[r]);
        REG_WRITE_FMT_BUF_32("\tldrb\t%s, [%s]\n", reglist[r], reglist[r]);
        break;

    case P_INTPTR:
        //fprintf(Outfile, "\tldr\t%s, [%s]\n", reglist[r], reglist[r]);
        REG_WRITE_FMT_BUF_32("\tldr\t%s, [%s]\n", reglist[r], reglist[r]);
        break;

    case P_LONGPTR:
        //fprintf(Outfile, "\tldr\t%s, [%s]\n", reglist[r], reglist[r]);
        REG_WRITE_FMT_BUF_32("\tldr\t%s, [%s]\n", reglist[r], reglist[r]);
        break;
    }
    return (r);
}

// <<
int register_arm_cgshlconst(REGISTER_CONTEXT_PARAM, int r, int val)
{
    // fprintf(Outfile, "\tlsl\t%s, %s, #%d\n", reglist[r], reglist[r], val);
    REG_WRITE_FMT_BUF_32("\tlsl\t%s, %s, #%d\n", reglist[r], reglist[r], val);
    return r;
}

// a = *b
int register_arm_cgstorederef(REGISTER_CONTEXT_PARAM, int r1, int r2, int type)
{
    char buf[32];
    switch (type)
    {
    case P_CHAR:
        // fprintf(Outfile, "\tstrb\t%s, [%s]\n", reglist[r1], reglist[r2]);
        snprintf(buf, 32, "\tstrb\t%s, [%s]\n", reglist[r1], reglist[r2]);
        REG_WRITE_BUF();
        break;

    case P_INT:
    case P_LONG:
        // fprintf(Outfile, "\tstr\t%s, [%s]\n", reglist[r1], reglist[r2]);
        snprintf(buf, 32, "\tstr\t%s, [%s]\n", reglist[r1], reglist[r2]);
        REG_WRITE_BUF();
        break;

    default:
        REG_PUBLISH_ERROR(ctx, "Can't cgstoderef on type:", type);
    }
    return (r1);
}

//gen label for store string.
void register_arm_cgglobstr(REGISTER_CONTEXT_PARAM, int label, const char *str){
    //TODO need impl
}

//given the label id of a global string. load its address into a new register.
int register_arm_cgloadglobstr(REGISTER_CONTEXT_PARAM, int id){
    //TODO need impl
    int reg = register_arm_alloc(ctx);

    return reg;
}

// & | ^  << >>
int register_arm_cgand(REGISTER_CONTEXT_PARAM, int r1, int r2){
     //TODO need impl
    return r1;
}
int register_arm_cgor(REGISTER_CONTEXT_PARAM, int r1, int r2){
  //TODO need impl
    return r1;
}

int register_arm_cgxor(REGISTER_CONTEXT_PARAM, int r1, int r2){
  //TODO need impl
    return r1;
}

int register_arm_cgshl(REGISTER_CONTEXT_PARAM, int r1, int r2){
  //TODO need impl
    return r1;
}
int register_arm_cgshr(REGISTER_CONTEXT_PARAM, int r1, int r2){
  //TODO need impl
    return r1;
}

// Negate a register's value
int register_arm_cgnegate(REGISTER_CONTEXT_PARAM, int r){
  //TODO need impl
    return r;
}

// Invert a register's value
int register_arm_cginvert(REGISTER_CONTEXT_PARAM, int r){
  //TODO need impl
    return r;
}
// Logically negate a register's value
int register_arm_cglognot(REGISTER_CONTEXT_PARAM, int r){
  //TODO need impl
    return r;
}

int register_arm_cgboolean(REGISTER_CONTEXT_PARAM, int r, int op, int label){
      //TODO need impl
    return r;
}