
#include "writer_file.h"

int writer_file_start(WriterContext *context, int initSize){
    const char* file = (char*)context->param;
    context->file = fopen(file, "wb+");
    if(context->file == NULL){
        fprintf(stderr, "writer_file_start: failed, path: %s\n", file);
        exit(1);
    }
    return context->file != NULL;
}

int writer_file_writeChars(WriterContext *context, const char *content){
    int res = fputs(content, context->file);
    if(res == EOF){
        printf("writer_file_writeChars: result = %d\n", res);
    }
    return res;
}

void writer_file_end(WriterContext *context){
    if(context->file != NULL){
        fclose(context->file);
        context->file = NULL;
    }
}
