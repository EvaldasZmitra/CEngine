#include "geometry.h"

const float EPSILON = 0.0000001;

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