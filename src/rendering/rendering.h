#ifndef RENDERING_H
#define RENDERING_H
#define GLEW_STATIC
#include "../../include/GL/glew.h"
#include "../../include/GLFW/glfw3.h"
#include <stdio.h>
#include "../math/linear_algebra.h"
#include "../math/geometry.h"

typedef struct Transform
{
    float *position;
    float *rotation;
    float *scale;
} Transform;

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
    char *file_name;
} Mesh;

typedef struct Entity
{
    unsigned int texture;
    char *name;
    Transform transform;
    unsigned int shader;
    Mesh *mesh;

    char *texture_name;
    char *vert_name;
    char *frag_name;

} Entity;

void invert_transform(Transform transform);
void line_cast(float *start, float *end, float *point, Entity *entities);
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
void free_mesh_gpu(Mesh *mesh);
void draw_entity(Entity *entity, Camera camera);
Camera create_default_camera();
void load_mesh_to_gpu(Mesh *mesh);
Entity *load_entities(char *text, int *num_entities);
void save_entities(const char *file_name, Entity *entities, unsigned int num_entities);
void create_mvp(
    float *position,
    float *rotation,
    float *scale,
    float *view_projection,
    float *out);
int raycast(float *origin, float *direction, Entity *entities, int num_entities);
#endif
