#include "common_headers.h"
#ifndef _REGISTER_WRITE_PRI_H
#define _REGISTER_WRITE_PRI_H
CPP_START

typedef struct
{   
    void *param;
    int type;
    char *buffer;
    int bufPos;
    FILE *file;
}WriterContext;

CPP_END

#endif