
#include "content_file.h"

int content_file_start(CONTENT_PARAM){
    const char* file = (char*)ctx->param;
    ctx->infile = fopen(file, "r");
    if(ctx->infile == NULL){
     printf("content_file_start: failed, path: %s\n", file);
    }
    return ctx->infile != NULL;
}

void content_file_end(CONTENT_PARAM){
    if(ctx->infile != NULL){
        fclose(ctx->infile);
        ctx->infile = NULL;
    }
}

int content_file_nextChar(CONTENT_PARAM){
    return fgetc(ctx->infile);
}