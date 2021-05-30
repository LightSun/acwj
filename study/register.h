#include "common_headers.h"
#ifndef _REGISTER_H
#define _REGISTER_H
#include "writer.h"
#include "register_pri.h"
CPP_START

#define REGISTER_TYPE_X64 1
#define REGISTER_TYPE_ARM64 2

#define REGISTER_ASSIGN_FUN(R, m, n) R->register_##m = register_##n##_##m;

#define REGISTER_ASSIGN_FUNCS(R, arch)\
REGISTER_ASSIGN_FUN(R, cgpreamble, arch)\
REGISTER_ASSIGN_FUN(R, cgpostamble, arch)\
REGISTER_ASSIGN_FUN(R, cgload, arch)\
REGISTER_ASSIGN_FUN(R, cgadd, arch)\
REGISTER_ASSIGN_FUN(R, cgsub, arch)\
REGISTER_ASSIGN_FUN(R, cgmul, arch)\
REGISTER_ASSIGN_FUN(R, cgdiv, arch)\
REGISTER_ASSIGN_FUN(R, cgprintint, arch)\
REGISTER_ASSIGN_FUN(R, cgloadglob, arch)\
REGISTER_ASSIGN_FUN(R, cgstoreglob, arch)\
REGISTER_ASSIGN_FUN(R, cgglobsym, arch)\
REGISTER_ASSIGN_FUN(R, free_all, arch)\
REGISTER_ASSIGN_FUN(R, cgequal, arch)\
REGISTER_ASSIGN_FUN(R, cgnotequal, arch)\
REGISTER_ASSIGN_FUN(R, cglessthan, arch)\
REGISTER_ASSIGN_FUN(R, cggreaterthan, arch)\
REGISTER_ASSIGN_FUN(R, cglessequal, arch)\
REGISTER_ASSIGN_FUN(R, cggreaterequal, arch)\
REGISTER_ASSIGN_FUN(R, cgjump, arch)\
REGISTER_ASSIGN_FUN(R, cglabel, arch)\
REGISTER_ASSIGN_FUN(R, cgcompare_and_jump, arch)\
REGISTER_ASSIGN_FUN(R, cgcompare_and_set, arch)\
REGISTER_ASSIGN_FUN(R, cgfuncpreamble, arch)\
REGISTER_ASSIGN_FUN(R, cgfuncpostamble, arch)\
REGISTER_ASSIGN_FUN(R, cgwiden, arch)\
REGISTER_ASSIGN_FUN(R, cgprimsize, arch)\
REGISTER_ASSIGN_FUN(R, cgcall, arch)\
REGISTER_ASSIGN_FUN(R, cgreturn, arch)

typedef struct _Register Register;
struct _Register
{
    RegisterContext* context;
    // Print out the assembly preamble
    void (*register_cgpreamble)(REGISTER_CONTEXT_PARAM);
    void (*register_cgpostamble)(REGISTER_CONTEXT_PARAM, int endLabel);

    int (*register_cgload)(REGISTER_CONTEXT_PARAM, int value);
    int (*register_cgadd)(REGISTER_CONTEXT_PARAM, int r1, int r2);
    int (*register_cgsub)(REGISTER_CONTEXT_PARAM, int r1, int r2);
    int (*register_cgmul)(REGISTER_CONTEXT_PARAM, int r1, int r2);
    int (*register_cgdiv)(REGISTER_CONTEXT_PARAM, int r1, int r2);

    void (*register_cgprintint)(REGISTER_CONTEXT_PARAM, int r);

    // Load a value from a variable into a register.
    // Return the number of the register
    //name: identifier name
    int (*register_cgloadglob)(REGISTER_CONTEXT_PARAM, int pType, const char *name);
    // Store a register's value into a variable
    //name: identifier name
    int (*register_cgstoreglob)(REGISTER_CONTEXT_PARAM, int r, int pType, const char *name);
    // Generate a global symbol
    //pType: one of Primitive types
    //name: identifier name
    void (*register_cgglobsym)(REGISTER_CONTEXT_PARAM, int pType, const char *name);

    void (*register_free_all)();

    //----------------- compare operator -------------------------
    int (*register_cgequal)(REGISTER_CONTEXT_PARAM, int r1, int r2);
    int (*register_cgnotequal)(REGISTER_CONTEXT_PARAM, int r1, int r2);
    int (*register_cglessthan)(REGISTER_CONTEXT_PARAM, int r1, int r2);
    int (*register_cggreaterthan)(REGISTER_CONTEXT_PARAM, int r1, int r2);

    int (*register_cglessequal)(REGISTER_CONTEXT_PARAM, int r1, int r2);
    int (*register_cggreaterequal)(REGISTER_CONTEXT_PARAM, int r1, int r2);

    //---------------- if statement ----------------
    // Generate a jump to a label
    void (*register_cgjump)(REGISTER_CONTEXT_PARAM, int l);
    // Generate a label
    void (*register_cglabel)(REGISTER_CONTEXT_PARAM, int l);

    int (*register_cgcompare_and_jump)(REGISTER_CONTEXT_PARAM, int asTop, int r1, int r2, int label);
    int (*register_cgcompare_and_set)(REGISTER_CONTEXT_PARAM, int asTop, int r1, int r2);

    //-------------- function -----------------------
    void (*register_cgfuncpreamble)(REGISTER_CONTEXT_PARAM, const char *funcName);

    void (*register_cgfuncpostamble)(REGISTER_CONTEXT_PARAM);

    //---------------- widen --------------------------
    int (*register_cgwiden)(REGISTER_CONTEXT_PARAM, int r, int oldtype, int newtype);

    //get the data size of the primitive type.
    int (*register_cgprimsize)(REGISTER_CONTEXT_PARAM, int pType);

    /**
 * To call a function with one argument, we need to copy the register with the argument value into %rdi.
 *  On return, we need to copy the returned value from %rax into the register that will have this new value:
 * */
    int (*register_cgcall)(REGISTER_CONTEXT_PARAM, int reg, const char *symName);

    //return
    void (*register_cgreturn)(REGISTER_CONTEXT_PARAM, int reg, int pType, int endlabel);
};

Register* register_new(int type);
void register_delete(Register* r);

CPP_END

#endif