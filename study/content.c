#include "content/content_text.h"
#include "content/content_file.h"
#include "content.h"

Content* content_new(int type,void* param){
    Content *delegate = (Content *)malloc(sizeof(Content));
    delegate->context = (ContentContext*)malloc(sizeof(ContentContext));
    memset(delegate->context, 0, sizeof(ContentContext));
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

    delegate->context->param = strdup(p);
    delegate->context->type = type;
    delegate->context->dumpId = 1;
    return delegate;
}
void content_delete(Content* cd){
    if(cd->context){
        if(cd->context->type == CONTENT_TYPE_FILE || cd->context->type == CONTENT_TYPE_TEXT){
            if(cd->context->param){
                free(cd->context->param);
                cd->context->param = NULL;
            }
        }
        free(cd->context);
        cd->context = NULL;
    }
    free(cd);
}