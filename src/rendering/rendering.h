#ifndef IO_H
#define IO_H
#include <stdio.h>
#include "../math/linear_algebra.h"
#include "../math/geometry.h"

char *read_file_stream(FILE *infile);
char *read_file(const char *file_name);
char *read_file_binary(const char *file_name);
#endif
