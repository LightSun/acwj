#ifndef _REGISTER_PRI_H
#define _REGISTER_PRI_H
#include "common_headers.h"
#include "sym.h"
CPP_START

#define REG_COUNT 4
#define REG_MAXINTS 1024
struct _Writer;

typedef struct
{
    struct _Writer *writer;
    int freereg[REG_COUNT]; // List of available registers 

    // We have to store large integer literal values in memory.
    // Keep a list of them which will be output in the postamble
    int intlist[REG_MAXINTS];
    int intslot;
}RegisterContext;

#define REGISTER_CONTEXT_PARAM RegisterContext *ctx
#define REG_G_WRITER(ctx) ctx->writer
#define REG_G_WRITER_CTX(ctx) ctx->writer->context
#define REG_G_GLOBAL_STATE(ctx) REG_G_WRITER_CTX(ctx)->globalState

#define REG_WRITE_BUF() REG_G_WRITER(ctx)->writeChars(REG_G_WRITER_CTX(ctx), buf)
#define REG_SYM_TABLE(ctx, id) sym_getGlob(REG_G_GLOBAL_STATE(ctx), id)
/* #define REG_WRITE_BUF(ctx, buf, fmt, ...) \
do{ \
va_list vArgList; \
va_start(vArgList, fmt);\
int i =_vsnprintf(buf, sizeof(buf), fmt, vArgList); \
va_end(vArgList); \ 
REG_G_WRITER(ctx)->writeChars(REG_G_WRITER_CTX(ctx), buf);\
}while(0); */

CPP_END
#endif