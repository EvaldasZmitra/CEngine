#ifndef IO_H
#define IO_H
#include <stdio.h>

char *read_file_stream(FILE *infile);
char *read_file(const char *file_name);
char *read_file_binary(const char *file_name);
#endif