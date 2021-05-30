
#include "writer_file.h"

int writer_file_start(WriterContext *context, int initSize){
    const char* file = (char*)context->param;
    context->file = fopen(file, "wb+");
    if(context->file == NULL){
        printf("writer_file_start: failed, path: %s\n", file);
    }
    return context->file != NULL;
}

int writer_file_writeChars(WriterContext *context, const char *content){
    return fputs(content, context->file);
}

void writer_file_end(WriterContext *context){
    if(context->file != NULL){
        fclose(context->file);
        context->file = NULL;
    }
}