#include "content_text.h"

int pos;

int content_text_start(CONTENT_PARAM){
    char * _content = (char*)param;
    pos = 0;
    return _content != NULL;
}

void content_text_end(CONTENT_PARAM){
    pos = 0;
}

int content_text_nextChar(CONTENT_PARAM){
    char * _content = (char*)param;
    if(pos >= strlen(_content)){
        return -1; //EOF
    }
    return _content[pos++];
}