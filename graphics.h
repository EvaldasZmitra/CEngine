#ifndef GRAPHICS_H
#define GRAPHICS_H
#define GLEW_STATIC
#include "./include/GL/glew.h"
#include "./include/GLFW/glfw3.h"
#include "util.h"

typedef enum VBO_TYPE
{
    VBO_VERTEX,
    VBO_NORMAL,
    VBO_UV,
    VBO_INDICES,
    MAX_VBOS,

    VAO,
    NUM_VERTICES,
    NUM_INDICES,
    SHADER,
    MAX_ATTRIBUTES
} VBO_TYPE;

typedef struct Camera
{
    float *position;
    float *forward;
    float fov;
    float aspect;
    float nearClipPlane;
    float farClipPlane;
} Camera;

typedef struct Entity
{
    char *name;
    float *position;
    float *rotation;
    float *scale;
    unsigned int *gpu_mesh;
} Entity;

GLFWwindow *create_window(int width, int height);

void error_callback(int error, const char *description);

unsigned int create_shader(const char *code, unsigned int type);
unsigned int create_shader_program(unsigned int fragment_shader, unsigned int vertex_shader);
unsigned int create_shader_program_from_code(const char *vertex_code, const char *fragment_code);

unsigned int create_vbo(const float *data, int size, int stride, int type);
void delete_gpu_mesh(unsigned int *mesh);

void create_camera();
void draw_gpu_mesh(unsigned int *obj);
void uniform_matrix_4x4(unsigned int shader, const float *matrix, char *name);

Camera create_default_camera();
void draw_entities(Entity *entities, int count, float *view_projection_m, GLFWwindow *window);
void load_shaded_mesh(unsigned int *out, Mesh mesh, char *vertex_shader_file, char *fragment_shader_file);
Entity *load_entities_from_text(char *text, int *num_entities);
unsigned int load_dds_to_gpu(
    unsigned char *buffer,
    unsigned int format,
    unsigned int mip_map_count,
    unsigned int width,
    unsigned int height);
#endif