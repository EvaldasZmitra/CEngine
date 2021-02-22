#ifndef GEOMETRY_H
#define GEOMETRY_H

int intersect_triangle(
    float *origin, float *direction,
    float *v0, float *v1, float *v2,
    float *distance);
float raycast_plane(float *origin, float *direction);

#endif