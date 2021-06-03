#include "global_context.h"
#include "content.h"
#include "writer.h"
#include "register.h"

void globalContext_publishError(GlobalContext *ctx, const char *format, ...)
{
    char buf[strlen(format) * 3];

    va_list vArgList;
    va_start(vArgList, format);
    int i = _vsnprintf(buf, sizeof(buf), format, vArgList);
    va_end(vArgList);

    fprintf(stderr, "%s\n on line %d\n", buf, ctx->content->context->line);
    exit(1);
}

void globalContext_init(GlobalContext* gCtx, struct GlobalState* gs, struct _Register* reg, struct _Content* cd, struct _Writer* w){
    gCtx->writer = w;
    gCtx->content = cd;
    gCtx->reg = reg;

    gs->gContext = gCtx;

    cd->context->globalState = gs;
    w->context->globalState = gs;
    cd->context->_register = reg;
    w->context->_register = reg;
    reg->context->writer = w;
}


void globalContext_destroy(GlobalContext *gCtx){
    struct GlobalState *gs = gCtx->content->context->globalState;
    content_delete(gCtx->content);
    writer_delete(gCtx->writer);

    sym_globalState_delete(gs);
    register_delete(gCtx->reg);
}