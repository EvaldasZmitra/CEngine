#include "util.h"
#include <stdlib.h>

char *read_file_stream(FILE *infile)
{
    char *buffer;
    long numbytes;
    fseek(infile, 0L, SEEK_END);
    numbytes = ftell(infile);
    fseek(infile, 0L, SEEK_SET);
    buffer = (char *)calloc(numbytes, sizeof(char));
    fread(buffer, sizeof(char), numbytes, infile);
    fclose(infile);
    return buffer;
}

char *read_file(const char *file_name)
{
    FILE *infile = fopen(file_name, "r");
    return read_file_stream(infile);
}

char *read_file_binary(const char *file_name)
{
    FILE *infile = fopen(file_name, "rb");
    return read_file_stream(infile);
}