
#include "writer_text.h"

static int LOAD_FACTOR = 2;

int writer_text_start(WriterContext *context,int initSize){
    if(initSize <= 0){
        initSize = 1024;
    }
    if(context->buffer == NULL){
        context->buffer = (char*)malloc(initSize + 1);
    }
    context->bufPos = 0;
    return 1;
}

int writer_text_writeChars(WriterContext *context,const char *content){
    int leftLen = strlen(context->buffer) - context->bufPos;
    int expectLen = strlen(content);
    if(expectLen <= leftLen){
        strcpy(context->buffer + context->bufPos, content);
    }else{
        char *oldBuffer = context->buffer;
        context->buffer = (char*)malloc((strlen(context->buffer) + expectLen + 1) * LOAD_FACTOR);
        //put old and new chars
        strcpy(context->buffer, oldBuffer);
        strcpy(context->buffer + context->bufPos, content);
        free(oldBuffer);
    }
    context->bufPos += expectLen;
    printf("buffer content: %s", context->buffer);
    return 1;
}

void writer_text_end(WriterContext *context){
    if(context->buffer){
        free(context->buffer);
        context->buffer = NULL;
    }
    context->bufPos = 0;
}