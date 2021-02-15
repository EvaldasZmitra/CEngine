#ifndef UTIL_H
#define UTIL_H

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
unsigned char *load_dds(
    const char *file,
    unsigned int *width,
    unsigned int *height,
    unsigned int *linear_size,
    unsigned int *mip_map_count,
    unsigned int *format,
    unsigned int *buffer_size);
char *read_file(char *file_name);
Mesh read_mesh(const char *file);
#endif
