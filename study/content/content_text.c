#include "content_text.h"

int content_text_start(CONTENT_PARAM){
    char * _content = (char*)ctx->param;
    ctx->pos = 0;
    return _content != NULL;
}

void content_text_end(CONTENT_PARAM){
    ctx->pos = 0;
}

int content_text_nextChar(CONTENT_PARAM){
    char * _content = (char*)ctx->param;
    if(ctx->pos >= strlen(_content)){
        return -1; //EOF
    }
    return _content[ctx->pos++];
}