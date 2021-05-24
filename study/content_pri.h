

#include "common_headers.h"

typedef struct{
    void *param;
    int type;
 //used for scanner
    int line;
    int putback;
} ContentContext;