#include "common_headers.h"
#ifndef _REGISTER_WRITE_PRI_H
#define _REGISTER_WRITE_PRI_H
CPP_START

struct GlobalState;

typedef struct
{   
    void *param;
    int type;
    
    char *buffer;
    int bufPos;
    FILE *file;

    int label; //the arm label
    struct GlobalState *globalState; // the global state. help do
}WriterContext;

CPP_END

#endif