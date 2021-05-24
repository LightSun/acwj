#include "content_text.h"
#include "content_file.h"

Content* content_new(int type,void* param){
    Content *delegate = (Content *)malloc(sizeof(Content));
    memset(delegate, 0, sizeof(Content));
    switch (type)
    {
    case CONTENT_TYPE_FILE:
        delegate->start = content_file_start;
        delegate->end = content_file_end;
        delegate->nextChar = content_file_nextChar;
        break;

    default:
    case CONTENT_TYPE_TEXT:
        delegate->start = content_text_start;
        delegate->end = content_text_end;
        delegate->nextChar = content_text_nextChar;
        break;
    }
    char *p = (char *)param;
    char* dst = (char*)malloc(strlen(p) + 1);
    strcpy(dst, p);
    dst[strlen(p)] = '\0';

    delegate->context.param = dst;
    delegate->context.type = type;
    return delegate;
}
void content_delete(Content* cd){
    if(cd->context.type == CONTENT_TYPE_FILE || cd->context.type == CONTENT_TYPE_TEXT){
        if(cd->context.param){
            free(cd->context.param);
        }
    }
    free(cd);
}