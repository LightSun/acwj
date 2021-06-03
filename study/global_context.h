#ifndef _CONTEXT_H
#define _CONTEXT_H

#include "common_headers.h"

CPP_START

struct _Content;
struct _Writer;
struct GlobalState;
struct _Register;

typedef struct _GlobalContext GlobalContext;
struct _GlobalContext{
  struct _Content *content;
  struct _Writer *writer;
  struct _Register *reg;
};

void globalContext_publishError(GlobalContext* ctx, const char* format, ...);

void globalContext_init(GlobalContext* ctx, struct GlobalState* gs, struct _Register* reg, 
    struct _Content* cd, struct _Writer* w);

void globalContext_destroy(GlobalContext *ctx);

CPP_END

#endif