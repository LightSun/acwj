
#include "content_file.h"

FILE *Infile;

int content_file_start(CONTENT_PARAM){
    const char* file = (char*)param;
    Infile = fopen(file, "r");
    return Infile != NULL;
}

void content_file_end(CONTENT_PARAM){
    if(Infile != NULL){
        fclose(Infile);
        Infile = NULL;
    }
}

int content_file_nextChar(CONTENT_PARAM){
    return fgetc(Infile);
}