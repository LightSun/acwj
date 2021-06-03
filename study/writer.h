
#ifndef _REGISTER_WRITE_H
#define _REGISTER_WRITE_H
#include "common_headers.h"
#include "writer_pri.h"
#include "global_context.h"
CPP_START
//#include "stdarg.h"

#define WRITER_G_REG(w) w->context->_register
#define WRITER_G_REG_CTX(w) w->context->_register->context 
#define WRITER_PUBLISH_ERROR(w, fmt, ...) globalContext_publishError(w->context->globalState->gContext, fmt, ##__VA_ARGS__)

#define WRITER_TYPE_FILE 1
#define WRITER_TYPE_TEXT 2

typedef struct _Writer Writer;
struct _Writer
{
    WriterContext *context;
    int (*start)(WriterContext *context, int initBufferSize);
    int (*writeChars)(WriterContext *context,const char *content);

    //void (*writeFmt)(WriterContext *context,const char *format,...);
    void (*end)(WriterContext *context);
};

Writer *writer_new(int type, void* param);
void writer_delete(Writer* writer);

CPP_END
#endif