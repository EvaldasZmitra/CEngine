#include <math.h>
#include <stdlib.h>
#include <stdio.h>

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
    matrix[15] = z;
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
        out[1] = copysignf(M_PI / 2, sinp); // use 90 degrees if out of range
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

void normalize(float *v)
{
    float sum = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
    float f = inverse_square_root(sum);
    v[0] *= f;
    v[1] *= f;
    v[2] *= f;
}

void cross(float *v1, float *v2, float *out)
{
    out[0] = v1[1] * v2[2] - v1[2] * v2[1];
    out[1] = v1[2] * v2[0] - v1[0] * v2[2];
    out[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

void subtract(float *v1, float *v2)
{
    v1[0] -= v2[0];
    v1[1] -= v2[1];
    v1[2] -= v2[2];
    v1[3] -= v2[3];
}

float dot(float *v1, float *v2)
{
    return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

void project(float fov, float aspect, float zNear, float zFar, float *out)
{
    float r = zFar - zNear;
    out[0] = 1.0f / (tan(fov / 2.0f) * aspect);
    out[5] = 1.0f / (tan(fov / 2.0f));
    out[10] = -(zFar + zNear) / r;
    out[14] = -1.0f;
    out[11] = -(2 * zNear * zFar) / r;
}

void view(float *pos, float *fwd, float *up, float *out)
{
    float right[] = {0, 0, 0};
    cross(fwd, up, right);
    cross(right, fwd, up);

    out[0] = right[0];
    out[1] = right[1];
    out[2] = right[2];
    out[3] = -dot(right, pos);

    out[4] = up[0];
    out[5] = up[1];
    out[6] = up[2];
    out[7] = -dot(up, pos);

    out[8] = fwd[0];
    out[9] = fwd[1];
    out[10] = fwd[2];
    out[11] = dot(fwd, pos);

    out[12] = 0;
    out[13] = 0;
    out[14] = 0;
    out[15] = 1;
}
