#ifndef _REGISTER_PRI_H
#define _REGISTER_PRI_H
#include "common_headers.h"
CPP_START

struct _Writer;

typedef struct
{
    struct _Writer *writer;
    
}RegisterContext;

#define REGISTER_CONTEXT_PARAM RegisterContext *ctx
#define REG_G_WRITER(ctx) ctx->writer
#define REG_G_WRITER_CTX(ctx) ctx->writer->context
#define REG_G_GLOBAL_STATE(ctx) ctx->writer->globalState

CPP_END
#endif