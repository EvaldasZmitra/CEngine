#include <stdio.h>
#include "game.h"
#include "graphics.h"

Entity *load_entities_from_file(char *file)
{
    char *out;
    int out_size;
    while (parse_file(file, out_size, out))
    {
        float *position;
        float *rotation;
        float *scale;
        float *mesh_file;
        float *vertex_shader_file;
        float *fragment_shader_file;
        float *vertices;
        unsigned int num_vertices;

        float gpu_mesh[MAX_ATTRIBUTES] = {0};
        create_gpu_mesh(vertices, num_vertices, gpu_mesh);
        Entity entity = {
            .mesh = gpu_mesh,
            .position = position,
            .scale = scale};
    }
}

int *parse_file(char *file, int *out_size, char *out)
{
    return 0;
}

int block_type(char *block)
{
    return 0;
}

char *read_file(char *file_name)
{
    FILE *infile = fopen(file_name, "r");
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
