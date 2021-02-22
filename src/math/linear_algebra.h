#ifndef LINEAR_ALGEBRA_H
#define LINEAR_ALGEBRA_H

void negate_vec3(float *vec);
void invert_vec3(float *vec);
void multiply_vec3(float *v, float s);
void cross_vector3(float *v1, float *v2, float *out);
float inverse_square_root(float number);
void normalize_vector3(float *v);
void subtract_vector3(float *v1, float *v2);
void add_vector3(float *v1, float *v2);
float dot_vector3(float *v1, float *v2);

void set_4x4_matrix_position(float *matrix, float x, float y, float z);
void set_4x4_matrix_scale(float *matrix, float x, float y, float z);
void quaterion_to_4x4_matrix(float *q, float *out);
void multiply_4x4_matrices(float *m1, float *m2, float *out);
void multiply_vec3_by_4x4_matrix(float *m, float *v, float *o);
void print_4x4_matrix(float *m);
/*
 * Pitch, yaw, roll
*/
void euler_to_quaternion(float *e, float *out);
void quaternion_to_euler(float *q, float *out);
void create_projection(float fov, float aspect, float zNear, float zFar, float *out);
void create_view(float *pos, float *fwd, float *up, float *out);
void create_transform(float *position, float *rotation, float *scale, float *out);
#endif