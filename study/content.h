

#ifndef _CONTENT_H
#define _CONTENT_H
#include "common_headers.h"
#include "content_pri.h"

CPP_START

#define CONTENT_G_REG(c) c->context->_register
#define CONTENT_G_REG_ctx(c) c->context->_register->context
#define CONTENT_TYPE_FILE 1
#define CONTENT_TYPE_TEXT 2

typedef struct _Content Content;
struct _Content{
    ContentContext* context;
    int (*start)(CONTENT_PARAM); //true if success
    void (*end)(CONTENT_PARAM);
    int (*nextChar)(CONTENT_PARAM);
};


Content* content_new(int type,void* param);
void content_delete(Content* cd);


CPP_END

#endif