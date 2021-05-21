#include "writer.h"
#include "writer_text.h"
#include "writer_file.h"

Writer *writer_new(int type, void* param){
    Writer *writer = (Writer *)malloc(sizeof(Writer));
    char *dstParam = NULL;
    switch (type)
    {
    case WRITER_TYPE_FILE:
        writer->start = writer_file_start;
        writer->end = writer_file_end;
        writer->writeChars = writer_file_writeChars;

        char *p = (char *)param;
        char* dstParam = (char*)malloc(strlen(p) + 1);
        strcpy(dstParam, p);
        dstParam[strlen(p)] = '\0';
        break;

    default:
    case WRITER_TYPE_TEXT:
        writer->start = writer_text_start;
        writer->end = writer_text_end;
        writer->writeChars = writer_text_writeChars;
        break;
    }

    writer->context = (WriterContext *)malloc(sizeof(WriterContext));
    memset(writer->context, 0, sizeof(WriterContext));
    writer->context->param = dstParam;
    writer->context->type = type;
    return writer;
}

void writer_delete(Writer* writer){
    if(writer->context->buffer){
        free(writer->context->buffer);
        writer->context->buffer = NULL;
    }
    if(writer->context->file){
        fclose(writer->context->file);
        writer->context->file = NULL;
    }
    free(writer->context);
    free(writer);
}