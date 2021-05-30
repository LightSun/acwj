#ifndef _CONTENT_FILE_H
#define _CONTENT_FILE_H
#include "../common_headers.h"
#include "../content_pri.h"

CPP_START

int content_file_start(CONTENT_PARAM);

void content_file_end(CONTENT_PARAM);

int content_file_nextChar(CONTENT_PARAM);

CPP_END

#endif