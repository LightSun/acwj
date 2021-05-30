#include "register_arm64.h"
#include "../ast.h"
#include "../sym.h"
#include "../utils.h"
#include "../writer.h"

// Allocate a free register. Return the number of
// the register. Die if no available registers.
static int register_alloc(REGISTER_CONTEXT_PARAM)
{
    for (int i = 0; i < REG_COUNT; i++)
    {
        if (ctx->freereg[i])
        {
            ctx->freereg[i] = 0;
            return (i);
        }
    }
    fprintf(stderr, "Out of registers");
    exit(1);
    return (NOREG); // Keep -Wall happy
}
// Return a register to the list of available registers.
// Check to see if it's not already there.
static void register_free(REGISTER_CONTEXT_PARAM, int reg)
{
    if (ctx->freereg[reg] != 0)
    {
        fprintf(stderr, "Error trying to free register = %d", reg);
        exit(1);
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
            fprintf(stderr, "Out of int slots in set_int_offset()");
            exit(1);
        }
        ctx->intlist[ctx->intslot++] = val;
    }
    // Load r3 with this offset
    //fprintf(Outfile, "\tldr\tr3, .L3+%d\n", offset);
    char buf[32];
    snprintf(buf, 32, "\tldr\tr3, .L3+%d\n", offset);
    REG_G_WRITER(ctx)->writeChars(REG_G_WRITER_CTX(ctx), buf);
}

// Determine the offset of a variable from the .L2
// label. Yes, this is inefficient code.
static void set_var_offset(REGISTER_CONTEXT_PARAM, int id)
{
    int offset = 0;
    // Walk the symbol table up to id.
    // Find S_VARIABLEs and add on 4 until
    // we get to our variable
    for (int i = 0; i < id; i++)
    {
        if (REG_G_GLOBAL_STATE(ctx)->syms[i].stype == S_VARIABLE)
            offset += 4;
    }
    // Load r3 with this offset
    // fprintf(Outfile, "\tldr\tr3, .L2+%d\n", offset);
    char buf[32];
    snprintf(buf, 32, "\tldr\tr3, .L2+%d\n", offset);
    REG_WRITE_BUF();
}

//------------------------------------------------------------------

void register_arm64_free_all(REGISTER_CONTEXT_PARAM)
{
    ctx->freereg[0] = ctx->freereg[1] = ctx->freereg[2] = ctx->freereg[3] = 1;
}
// List of available registers and their names.
//static int freereg[4]; //move to context
static char *reglist[REG_COUNT] = {"r4", "r5", "r6", "r7"};

// Print out the assembly preamble
void register_arm64_cgpreamble(REGISTER_CONTEXT_PARAM)
{
    register_arm64_free_all(ctx);
    REG_G_WRITER(ctx)->writeChars(REG_G_WRITER_CTX(ctx), "\t.text\n");
    // fputs("\t.text\n", Outfile);
}

void register_arm64_cgpostamble(REGISTER_CONTEXT_PARAM, int endLabel)
{
    // Print out the global variables
    REG_G_WRITER(ctx)->writeChars(REG_G_WRITER_CTX(ctx), ".L2:\n");
    char buf[32];
    for (int i = 0; i < REG_G_GLOBAL_STATE(ctx)->globs; i++)
    {
        if (REG_G_GLOBAL_STATE(ctx)->syms[i].stype == S_VARIABLE)
        {
            snprintf(buf, 32, "\t.word %s\n", REG_G_GLOBAL_STATE(ctx)->syms[i].name);
            REG_G_WRITER(ctx)->writeChars(REG_G_WRITER_CTX(ctx), buf);
        }
    }
    // Print out the integer literals
    REG_G_WRITER(ctx)->writeChars(REG_G_WRITER_CTX(ctx), ".L3:\n");
    for (int i = 0; i < ctx->intslot; i++)
    {
        // fprintf(Outfile, "\t.word %d\n", Intlist[i]);
        snprintf(buf, 32, "\t.word %d\n", ctx->intlist[i]);
        REG_G_WRITER(ctx)->writeChars(REG_G_WRITER_CTX(ctx), buf);
    }
}

int register_arm64_cgload(REGISTER_CONTEXT_PARAM, int value)
{
    // Get a new register
    int r = register_alloc(ctx);

    char buf[32];
    // If the literal value is small, do it with one instruction
    if (value <= 1000)
        snprintf(buf, 32, "\tmov\t%s, #%d\n", reglist[r], value);
    else
    {
        set_int_offset(ctx, value);
        snprintf(buf, 32, "\tldr\t%s, [r3]\n", reglist[r]);
    }
    REG_G_WRITER(ctx)->writeChars(REG_G_WRITER_CTX(ctx), buf);
    return (r);
}
int register_arm64_cgadd(REGISTER_CONTEXT_PARAM, int r1, int r2);
int register_arm64_cgsub(REGISTER_CONTEXT_PARAM, int r1, int r2);
int register_arm64_cgmul(REGISTER_CONTEXT_PARAM, int r1, int r2);
int register_arm64_cgdiv(REGISTER_CONTEXT_PARAM, int r1, int r2);

void register_arm64_cgprintint(REGISTER_CONTEXT_PARAM, int r);

// Load a value from a variable into a register.
// Return the number of the register
//name: identifier name
int register_arm64_cgloadglob(REGISTER_CONTEXT_PARAM, int pType, const char *name)
{
    int r = register_alloc(ctx);
    // Get the offset to the variable
    set_var_offset(id);
    fprintf(Outfile, "\tldr\t%s, [r3]\n", reglist[r]);
    return (r);
}
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
void register_arm64_cgfuncpreamble(REGISTER_CONTEXT_PARAM, const char *funcName)
{
    //  char *name = Gsym[id].name;
    char buf[96];

    snprintf(buf, 96,
             "\t.text\n"
             "\t.globl\t%s\n"
             "\t.type\t%s, \%%function\n"
             "%s:\n"
             "\tpush\t{fp, lr}\n"
             "\tadd\tfp, sp, #4\n"
             "\tsub\tsp, sp, #8\n"
             "\tstr\tr0, [fp, #-8]\n",
             funcName, funcName, funcName);
    REG_G_WRITER(ctx)->writeChars(REG_G_WRITER_CTX(ctx), buf);
}

void register_arm64_cgfuncpostamble(REGISTER_CONTEXT_PARAM, int endLabel)
{
    register_arm64_cglabel(ctx, endLabel);
    // fputs("\tsub\tsp, fp, #4\n" "\tpop\t{fp, pc}\n" "\t.align\t2\n", Outfile);
    REG_G_WRITER(ctx)->writeChars(REG_G_WRITER_CTX(ctx), "\tsub\tsp, fp, #4\n"
                                                         "\tpop\t{fp, pc}\n"
                                                         "\t.align\t2\n");
}

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