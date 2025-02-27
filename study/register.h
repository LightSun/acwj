#include "common_headers.h"
#ifndef _REGISTER_H
#define _REGISTER_H
#include "register_pri.h"
#include "writer.h"
CPP_START

#define REGISTER_TYPE_X64 1
#define REGISTER_TYPE_ARM 2

#define REGISTER_ASSIGN_FUN(R, m, n) R->register_##m = register_##n##_##m;

#define REGISTER_ASSIGN_FUNCS(R, arch)               \
    REGISTER_ASSIGN_FUN(R, cgpreamble, arch)         \
    REGISTER_ASSIGN_FUN(R, cgpostamble, arch)        \
    REGISTER_ASSIGN_FUN(R, cgloadint, arch)          \
    REGISTER_ASSIGN_FUN(R, cgadd, arch)              \
    REGISTER_ASSIGN_FUN(R, cgsub, arch)              \
    REGISTER_ASSIGN_FUN(R, cgmul, arch)              \
    REGISTER_ASSIGN_FUN(R, cgdiv, arch)              \
    REGISTER_ASSIGN_FUN(R, cgprintint, arch)         \
    REGISTER_ASSIGN_FUN(R, cgloadglob, arch)         \
    REGISTER_ASSIGN_FUN(R, cgstoreglob, arch)        \
    REGISTER_ASSIGN_FUN(R, cgglobsym, arch)          \
    REGISTER_ASSIGN_FUN(R, free_all, arch)           \
    REGISTER_ASSIGN_FUN(R, cgjump, arch)             \
    REGISTER_ASSIGN_FUN(R, cglabel, arch)            \
    REGISTER_ASSIGN_FUN(R, cgcompare_and_jump, arch) \
    REGISTER_ASSIGN_FUN(R, cgcompare_and_set, arch)  \
    REGISTER_ASSIGN_FUN(R, cgfuncpreamble, arch)     \
    REGISTER_ASSIGN_FUN(R, cgfuncpostamble, arch)    \
    REGISTER_ASSIGN_FUN(R, cgwiden, arch)            \
    REGISTER_ASSIGN_FUN(R, cgprimsize, arch)         \
    REGISTER_ASSIGN_FUN(R, cgcall, arch)             \
    REGISTER_ASSIGN_FUN(R, cgreturn, arch)           \
    REGISTER_ASSIGN_FUN(R, cgaddress, arch)          \
    REGISTER_ASSIGN_FUN(R, cgderef, arch)            \
    REGISTER_ASSIGN_FUN(R, cgshlconst, arch)         \
    REGISTER_ASSIGN_FUN(R, cgstorederef, arch)       \
    REGISTER_ASSIGN_FUN(R, cgglobstr, arch)          \
    REGISTER_ASSIGN_FUN(R, cgloadglobstr, arch)      \
    REGISTER_ASSIGN_FUN(R, cgand, arch)              \
    REGISTER_ASSIGN_FUN(R, cgor, arch)               \
    REGISTER_ASSIGN_FUN(R, cgxor, arch)              \
    REGISTER_ASSIGN_FUN(R, cgshl, arch)              \
    REGISTER_ASSIGN_FUN(R, cgshr, arch)              \
    REGISTER_ASSIGN_FUN(R, cgnegate, arch)           \
    REGISTER_ASSIGN_FUN(R, cginvert, arch)           \
    REGISTER_ASSIGN_FUN(R, cglognot, arch)           \
    REGISTER_ASSIGN_FUN(R, cgboolean, arch)          \
    REGISTER_ASSIGN_FUN(R, cgtextseg, arch)          \
    REGISTER_ASSIGN_FUN(R, cgdataseg, arch)          \
    REGISTER_ASSIGN_FUN(R, cgloadlocal, arch)        \
    REGISTER_ASSIGN_FUN(R, cgstorelocal, arch)

typedef struct _Register Register;
struct _Register
{
    RegisterContext *context;

    void (*register_free_all)(REGISTER_CONTEXT_PARAM);

    // Print out the assembly preamble
    void (*register_cgpreamble)(REGISTER_CONTEXT_PARAM);
    void (*register_cgpostamble)(REGISTER_CONTEXT_PARAM, int sym_id);

    // Load an integer literal value into a register.
    // Return the number of the register
    int (*register_cgloadint)(REGISTER_CONTEXT_PARAM, int value, int type);
    int (*register_cgadd)(REGISTER_CONTEXT_PARAM, int r1, int r2);
    int (*register_cgsub)(REGISTER_CONTEXT_PARAM, int r1, int r2);
    int (*register_cgmul)(REGISTER_CONTEXT_PARAM, int r1, int r2);
    int (*register_cgdiv)(REGISTER_CONTEXT_PARAM, int r1, int r2);

    void (*register_cgprintint)(REGISTER_CONTEXT_PARAM, int r);

    // Load a value from a variable into a register.
    // Return the number of the register
    //name: identifier name
    int (*register_cgloadglob)(REGISTER_CONTEXT_PARAM, int sym_id, int op);
    // Store a register's value into a variable
    //name: identifier name
    int (*register_cgstoreglob)(REGISTER_CONTEXT_PARAM, int r, int sym_id);
    // Generate a global symbol
    //pType: one of Primitive types
    //name: identifier name
    void (*register_cgglobsym)(REGISTER_CONTEXT_PARAM, int sym_id);

    //----------------- compare operator -------------------------
    /*  int (*register_cgequal)(REGISTER_CONTEXT_PARAM, int r1, int r2);
    int (*register_cgnotequal)(REGISTER_CONTEXT_PARAM, int r1, int r2);
    int (*register_cglessthan)(REGISTER_CONTEXT_PARAM, int r1, int r2);
    int (*register_cggreaterthan)(REGISTER_CONTEXT_PARAM, int r1, int r2);

    int (*register_cglessequal)(REGISTER_CONTEXT_PARAM, int r1, int r2);
    int (*register_cggreaterequal)(REGISTER_CONTEXT_PARAM, int r1, int r2); */

    //---------------- if statement ----------------
    // Generate a jump to a label
    void (*register_cgjump)(REGISTER_CONTEXT_PARAM, int l);
    // Generate a label
    void (*register_cglabel)(REGISTER_CONTEXT_PARAM, int l);

    int (*register_cgcompare_and_jump)(REGISTER_CONTEXT_PARAM, int asTop, int r1, int r2, int label);
    int (*register_cgcompare_and_set)(REGISTER_CONTEXT_PARAM, int asTop, int r1, int r2);

    //-------------- function -----------------------
    void (*register_cgfuncpreamble)(REGISTER_CONTEXT_PARAM, int sym_id);

    void (*register_cgfuncpostamble)(REGISTER_CONTEXT_PARAM, int sym_id);

    //---------------- widen --------------------------
    int (*register_cgwiden)(REGISTER_CONTEXT_PARAM, int r, int oldtype, int newtype);

    //get the data size of the primitive type.
    int (*register_cgprimsize)(REGISTER_CONTEXT_PARAM, int pType);

    /**
 * To call a function with one argument, we need to copy the register with the argument value into %rdi.
 *  On return, we need to copy the returned value from %rax into the register that will have this new value:
 * */
    int (*register_cgcall)(REGISTER_CONTEXT_PARAM, int reg, int sym_id);

    //return
    void (*register_cgreturn)(REGISTER_CONTEXT_PARAM, int reg, int sym_id);

    //get addr
    int (*register_cgaddress)(REGISTER_CONTEXT_PARAM, int id);
    // de - ref-addr
    int (*register_cgderef)(REGISTER_CONTEXT_PARAM, int r, int pType);

    // <<
    int (*register_cgshlconst)(REGISTER_CONTEXT_PARAM, int r, int val);

    //assigning to an identifier through a pointer. a = *b
    int (*register_cgstorederef)(REGISTER_CONTEXT_PARAM, int leftReg, int rightReg, int type);

    //allocate label for string
    void (*register_cgglobstr)(REGISTER_CONTEXT_PARAM, int label, const char *str);

    //given the label id of a global string. load its address into a new register.
    int (*register_cgloadglobstr)(REGISTER_CONTEXT_PARAM, int id);

    // & | ^  <<  >>
    int (*register_cgand)(REGISTER_CONTEXT_PARAM, int r1, int r2);
    int (*register_cgor)(REGISTER_CONTEXT_PARAM, int r1, int r2);
    int (*register_cgxor)(REGISTER_CONTEXT_PARAM, int r1, int r2);
    int (*register_cgshl)(REGISTER_CONTEXT_PARAM, int r1, int r2);
    int (*register_cgshr)(REGISTER_CONTEXT_PARAM, int r1, int r2);
    // Negate a register's value
    int (*register_cgnegate)(REGISTER_CONTEXT_PARAM, int r);
    // Invert a register's value
    int (*register_cginvert)(REGISTER_CONTEXT_PARAM, int r);
    // Logically negate a register's value
    int (*register_cglognot)(REGISTER_CONTEXT_PARAM, int r);

    int (*register_cgboolean)(REGISTER_CONTEXT_PARAM, int r, int op, int label);

    //get local variable offset
    //int (*register_cggetlocaloffset)(REGISTER_CONTEXT_PARAM, int type, int isParam);

    //reset local offset
    //void (*register_cgresetlocals)(REGISTER_CONTEXT_PARAM);

    //the text and data seg in asm
    void (*register_cgtextseg)(REGISTER_CONTEXT_PARAM);
    void (*register_cgdataseg)(REGISTER_CONTEXT_PARAM);

    // Load a value from a local variable into a register.
    // Return the number of the register. If the
    // operation is pre- or post-increment/decrement,
    // also perform this action.
    int (*register_cgloadlocal)(REGISTER_CONTEXT_PARAM, int id, int op);

    // Store a register's value into a local variable
    int (*register_cgstorelocal)(REGISTER_CONTEXT_PARAM, int r, int id);
};

Register *register_new(int type);
void register_delete(Register *r);

CPP_END

#endif