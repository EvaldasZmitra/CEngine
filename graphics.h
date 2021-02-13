#ifndef GRAPHICS_H
#define GRAPHICS_H
#define GLEW_STATIC
#include "./include/GL/glew.h"
#include "./include/GLFW/glfw3.h"

typedef enum VBO_TYPE
{
    VBO_VERTEX,
    MAX_VBOS,
    VAO,
    NUM_VERTICES,
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
void create_gpu_mesh(const float *vertices, int num_vertices, unsigned int *out);
void delete_gpu_mesh(unsigned int *mesh);

void create_camera();
void draw_gpu_mesh(unsigned int *obj);
void uniform_matrix_4x4(unsigned int shader, const float *matrix, char *name);

Camera create_default_camera();
void draw(Entity *entities, int count, float *view_projection_m, GLFWwindow *window);
void load_shaded_mesh(unsigned int *out, char *vertex_shader_file, char *fragment_shader_file);
Entity *load_entities_from_text(char *text, int *num_entities);

#endif