#include "util.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "../include/cglm/cglm.h"

const float EPSILON = 0.0000001;

void negate_vec3(float *vec)
{
    vec[0] = -vec[0];
    vec[1] = -vec[1];
    vec[2] = -vec[2];
}

void invert_vec3(float *vec)
{
    vec[0] = 1 / vec[0];
    vec[1] = 1 / vec[1];
    vec[2] = 1 / vec[2];
}

void invert_transform(Transform transform)
{
    negate_vec3(transform.position);
    negate_vec3(transform.rotation);
    invert_vec3(transform.scale);
}

void multiply_vec3(float *v, float s)
{
    v[0] *= s;
    v[1] *= s;
    v[2] *= s;
}

void write_vec3(FILE *fp, float *vec3)
{
    fprintf(fp, "%f %f %f\n",
            vec3[0],
            vec3[1],
            vec3[2]);
}

void set_4x4_matrix_position(float *matrix, float x, float y, float z)
{
    matrix[0] = 1;
    matrix[5] = 1;
    matrix[10] = 1;
    matrix[15] = 1;

    matrix[3] = x;
    matrix[7] = y;
    matrix[11] = z;
}

void set_4x4_matrix_scale(float *matrix, float x, float y, float z)
{
    matrix[0] = x;
    matrix[5] = y;
    matrix[10] = z;
    matrix[15] = 1;
}

void quaterion_to_4x4_matrix(float *q, float *out)
{
    out[0] = 1.0f - 2.0f * q[1] * q[1] - 2.0f * q[2] * q[2];
    out[1] = 2.0f * q[0] * q[1] - 2.0f * q[2] * q[3];
    out[2] = 2.0f * q[0] * q[2] + 2.0f * q[1] * q[3];
    out[3] = 0.0f;

    out[4] = 2.0f * q[0] * q[1] + 2.0f * q[2] * q[3];
    out[5] = 1.0f - 2.0f * q[0] * q[0] - 2.0f * q[2] * q[2];
    out[6] = 2.0f * q[1] * q[2] - 2.0f * q[0] * q[3];
    out[7] = 0;

    out[8] = 2.0f * q[0] * q[2] - 2.0f * q[1] * q[3];
    out[9] = 2.0f * q[1] * q[2] + 2.0f * q[0] * q[3];
    out[10] = 1.0f - 2.0f * q[0] * q[0] - 2.0f * q[1] * q[1];
    out[11] = 0;

    out[12] = 0;
    out[13] = 0;
    out[14] = 0;
    out[15] = 1;
}

void multiply_4x4_matrices(float *m1, float *m2, float *out)
{
    for (int i = 0; i < 4; i++)
    {
        for (int y = 0; y < 4; y++)
        {
            int c = i * 4 + y;
            float sum = 0;
            for (int u = 0; u < 4; u++)
            {
                sum += m1[i * 4 + u] * m2[y + u * 4];
            }
            out[c] = sum;
        }
    }
}

void multiply_vec3_by_4x4_matrix(float *m, float *v, float *o)
{
    for (int y = 0; y < 4; y++)
    {
        float sum = 0;
        for (int x = 0; x < 4; x++)
        {
            sum += m[y * 4 + x] * v[x];
        }
        o[y] = sum;
    }
}

void print_4x4_matrix(float *m)
{
    for (int i = 0; i < 4; i++)
    {
        for (int y = 0; y < 4; y++)
        {
            float v = m[i * 4 + y];
            printf("%0.2f ", v);
        }
        printf("\n");
    }
    printf("\n");
}

/*
 * Pitch, yaw, roll
*/
void euler_to_quaternion(float *e, float *out)
{
    float cy = cos(e[0] * 0.5);
    float sy = sin(e[0] * 0.5);

    float cp = cos(e[1] * 0.5);
    float sp = sin(e[1] * 0.5);

    float cr = cos(e[2] * 0.5);
    float sr = sin(e[2] * 0.5);

    out[0] = cr * cp * cy + sr * sp * sy;
    out[1] = sr * cp * cy - cr * sp * sy;
    out[2] = cr * sp * cy + sr * cp * sy;
    out[3] = cr * cp * sy - sr * sp * cy;
}

void quaternion_to_euler(float *q, float *out)
{
    float sinr_cosp = 2 * (q[3] * q[0] + q[1] * q[2]);
    float cosr_cosp = 1 - 2 * (q[0] * q[0] + q[1] * q[1]);
    out[0] = (float)atan2(sinr_cosp, cosr_cosp);

    float sinp = 2 * (q[3] * q[1] - q[2] * q[0]);
    if (abs(sinp) >= 1)
        out[1] = copysignf(M_PI_2, sinp); // use 90 degrees if out of range
    else
        out[1] = (float)asin(sinp);

    float siny_cosp = 2 * (q[3] * q[2] + q[0] * q[1]);
    float cosy_cosp = 1 - 2 * (q[1] * q[1] + q[2] * q[2]);
    out[2] = atan2(siny_cosp, cosy_cosp);
}

float inverse_square_root(float number)
{
    long i;
    float x2, y;
    const float threehalfs = 1.5F;

    x2 = number * 0.5F;
    y = number;
    i = *(long *)&y;
    i = 0x5f3759df - (i >> 1);
    y = *(float *)&i;
    y = y * (threehalfs - (x2 * y * y));
    return y;
}

void normalize_vector3(float *v)
{
    float sum = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
    float f = inverse_square_root(sum);
    v[0] *= f;
    v[1] *= f;
    v[2] *= f;
}

void cross_vector3(float *v1, float *v2, float *out)
{
    out[0] = v1[1] * v2[2] - v1[2] * v2[1];
    out[1] = v1[2] * v2[0] - v1[0] * v2[2];
    out[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

void subtract_vector3(float *v1, float *v2)
{
    v1[0] -= v2[0];
    v1[1] -= v2[1];
    v1[2] -= v2[2];
}

void add_vector3(float *v1, float *v2)
{
    v1[0] += v2[0];
    v1[1] += v2[1];
    v1[2] += v2[2];
}

float dot_vector3(float *v1, float *v2)
{
    return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

void create_projection(float fov, float aspect, float zNear, float zFar, float *out)
{
    float r = zFar - zNear;
    out[0] = 1.0f / (tan(fov / 2.0f) * aspect);
    out[5] = 1.0f / (tan(fov / 2.0f));
    out[10] = -(zFar + zNear) / r;
    out[14] = -1.0f;
    out[11] = -(2 * zNear * zFar) / r;
}

void create_view(float *pos, float *fwd, float *up, float *out)
{
    float right[] = {0, 0, 0};
    cross_vector3(fwd, up, right);
    normalize_vector3(right);
    cross_vector3(right, fwd, up);
    normalize_vector3(up);

    out[0] = right[0];
    out[1] = right[1];
    out[2] = right[2];
    out[3] = -dot_vector3(right, pos);

    out[4] = up[0];
    out[5] = up[1];
    out[6] = up[2];
    out[7] = -dot_vector3(up, pos);

    out[8] = -fwd[0];
    out[9] = -fwd[1];
    out[10] = -fwd[2];
    out[11] = dot_vector3(fwd, pos);

    out[12] = 0;
    out[13] = 0;
    out[14] = 0;
    out[15] = 1;
}

void create_transform(float *position, float *rotation, float *scale, float *out)
{
    float scale_matrix[16] = {0};
    float rotate_matrix[16] = {0};
    float translate_matrix[16] = {0};
    float quaternion[4] = {0};
    set_4x4_matrix_scale(scale_matrix, scale[0], scale[1], scale[2]);
    euler_to_quaternion(
        (float[]){
            rotation[0],
            rotation[1],
            rotation[2]},
        quaternion);
    quaterion_to_4x4_matrix(quaternion, rotate_matrix);
    set_4x4_matrix_position(
        translate_matrix,
        position[0],
        position[1],
        position[2]);

    float translate_rotate[16] = {0};
    multiply_4x4_matrices(translate_matrix, rotate_matrix, translate_rotate);
    multiply_4x4_matrices(translate_rotate, scale_matrix, out);
}

void create_mvp(
    float *position,
    float *rotation,
    float *scale,
    float *view_projection,
    float *out)
{
    float transform[16] = {0};
    create_transform(position, rotation, scale, transform);
    multiply_4x4_matrices(view_projection, transform, out);
}

int intersect_triangle(
    float *origin, float *direction,
    float *v0, float *v1, float *v2,
    float *distance)
{
    float v1v0[3] = {v1[0], v1[1], v1[2]};
    float v2v0[3] = {v2[0], v2[1], v2[2]};
    subtract_vector3(v1v0, v0);
    subtract_vector3(v2v0, v0);
    float pvec[3] = {0};
    cross_vector3(direction, v2v0, pvec);
    float det = dot_vector3(v1v0, pvec);
    if (fabs(det) < EPSILON)
        return -1;
    float inv_det = 1.0f / det;
    float tvec[3] = {origin[0], origin[1], origin[2]};
    subtract_vector3(tvec, v0);
    float u = dot_vector3(tvec, pvec) * inv_det;
    if (u < 0 || u > 1)
        return -1;
    float qvec[3] = {tvec[0], tvec[1], tvec[2]};
    cross_vector3(tvec, v1v0, qvec);
    float v = dot_vector3(direction, qvec) * inv_det;
    if (v < 0 || u + v > 1)
        return -1;
    (*distance) = dot_vector3(v2v0, qvec) * inv_det;
    return 1;
}

float raycast_plane(float *origin, float *direction)
{
    float normal[] = {0, 1, 0};
    float d = dot_vector3(direction, normal);
    if (d < -EPSILON)
    {
        float to_plane_origin[3] = {0};
        subtract_vector3(to_plane_origin, origin);
        return dot_vector3(to_plane_origin, normal) / d;
    }
    return -1;
}