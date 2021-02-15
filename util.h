#ifndef UTIL_H
#define UTIL_H
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
    unsigned int texture;
    char *name;
    float *position;
    float *rotation;
    float *scale;
    unsigned int *gpu_mesh;
} Entity;

typedef struct Mesh
{
    unsigned int offset;
    unsigned int num_vertices;
    unsigned int num_indices;
    float *vertices;
    float *normals;
    float *uvs;
    unsigned int *indices;
} Mesh;

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
unsigned int load_dds_to_gpu();
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
void normalize(float *v);
void cross(float *v1, float *v2, float *out);
void subtract(float *v1, float *v2);
float dot(float *v1, float *v2);
void m_mat(float *m, float *v);
void project(float fov, float aspect, float zNear, float zFar, float *out);
void view(float *pos, float *fwd, float *up, float *out);
void create_transform(float *position, float *rotation, float *scale, float *out);
void create_mvp(
    float *position,
    float *rotation,
    float *scale,
    float *view_projection,
    float *out);
char *read_file(char *file_name);
Mesh read_mesh(const char *file);
#endif
