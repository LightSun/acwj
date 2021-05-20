

#ifndef _CONTENT_DELEGATE
#define _CONTENT_DELEGATE
#include "common_headers.h"

CPP_START

#define CONTENT_PARAM void* param
#define CONTENT_TYPE_FILE 1
#define CONTENT_TYPE_TEXT 2

typedef struct _ContentDelegate ContentDelegate;
struct _ContentDelegate{
    int type;
    void *param;
    int (*start)(CONTENT_PARAM); //true if success
    void (*end)(CONTENT_PARAM);
    int (*nextChar)(CONTENT_PARAM);
};

ContentDelegate* content_newDelegate(int type,void* param);
void content_deleteDelegate(ContentDelegate* cd);

CPP_END

#endif