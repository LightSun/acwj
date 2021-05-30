
#ifndef _WRITER_FILE_H
#define _WRITER_FILE_H
#include "../common_headers.h"
#include "../writer_pri.h"
CPP_START

int writer_file_start(WriterContext *context,int initSize);

int writer_file_writeChars(WriterContext *context,const char *content);

//void writer_file_writeFmt(WriterContext *context,const char *format,...);

void writer_file_end(WriterContext *context);
CPP_END
#endif