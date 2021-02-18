#ifndef UTIL_H
#define UTIL_H
#define GLEW_STATIC
#include "../include/GL/glew.h"
#include "../include/GLFW/glfw3.h"
#include "util.h"
#include <stdio.h>

typedef struct Camera
{
    float *position;
    float *forward;
    float fov;
    float aspect;
    float nearClipPlane;
    float farClipPlane;
} Camera;

typedef struct Mesh
{
    unsigned int vao;
    unsigned int vbo_vertices;
    unsigned int vbo_normal;
    unsigned int vbo_uv;
    unsigned int vbo_indices;
    unsigned int num_vertices;
    unsigned int num_indices;

    unsigned int offset;
    float *vertices;
    float *normals;
    float *uvs;
    unsigned int *indices;
} Mesh;

typedef struct Entity
{
    unsigned int texture;
    char *name;
    float *position;
    float *rotation;
    float *scale;
    unsigned int shader;
    Mesh *mesh;
} Entity;

char *read_file_stream(FILE *infile);
char *read_file(const char *file_name);
char *read_file_binary(const char *file_name);
unsigned int load_dds(const char *path);
GLFWwindow *create_window(int width, int height);
unsigned int create_shader(const char *code, unsigned int type);
unsigned int create_shader_program(unsigned int fragment_shader, unsigned int vertex_shader);
unsigned int create_shader_program_from_code(const char *vertex_code, const char *fragment_code);
void uniform_matrix_4x4(unsigned int shader, const float *matrix, char *name);
unsigned int create_vbo(const void *data, int size, int stride, int type);
void free_entity_gpu(Entity entity);
void draw_entity(Entity *entity, Camera camera);
Camera create_default_camera();
void load_entity_to_gpu(Entity *entity);
Entity *load_entities(char *text, int *num_entities);
void set_4x4_matrix_position(float *matrix, float x, float y, float z);
void set_4x4_matrix_scale(float *matrix, float x, float y, float z);
void quaterion_to_4x4_matrix(float *q, float *out);
void multiply_4x4_matrices(float *m1, float *m2, float *out);
void print_4x4_matrix(float *m);
/*
 * Pitch, yaw, roll
*/
void euler_to_quaternion(float *e, float *out);
void quaternion_to_euler(float *q, float *out);
float inverse_square_root(float number);
void normalize_vector3(float *v);
void cross_vector3(float *v1, float *v2, float *out);
void subtract_vector3(float *v1, float *v2);
float dot_vector3(float *v1, float *v2);
void create_projection(float fov, float aspect, float zNear, float zFar, float *out);
void create_view(float *pos, float *fwd, float *up, float *out);
void create_transform(float *position, float *rotation, float *scale, float *out);
void create_mvp(
    float *position,
    float *rotation,
    float *scale,
    float *view_projection,
    float *out);
#endif
