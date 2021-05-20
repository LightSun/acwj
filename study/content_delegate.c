#include "content_text.h"
#include "content_file.h"

ContentDelegate* content_newDelegate(int type,void* param){
    ContentDelegate *delegate = (ContentDelegate *)malloc(sizeof(ContentDelegate));
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
    delegate->param = dst;
    delegate->type = type;
    return delegate;
}
void content_deleteDelegate(ContentDelegate* cd){
    if(cd->type == CONTENT_TYPE_FILE || cd->type == CONTENT_TYPE_TEXT){
        if(cd->param){
            free(cd->param);
        }
    }
    free(cd);
}