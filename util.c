#include "util.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char *read_file_stream(FILE *infile)
{
    char *buffer;
    long numbytes;
    fseek(infile, 0L, SEEK_END);
    numbytes = ftell(infile);
    fseek(infile, 0L, SEEK_SET);
    buffer = (char *)calloc(numbytes, sizeof(char));
    fread(buffer, sizeof(char), numbytes, infile);
    fclose(infile);
    return buffer;
}

char *read_file(const char *file_name)
{
    FILE *infile = fopen(file_name, "r");
    return read_file_stream(infile);
}

char *read_file_binary(const char *file_name)
{
    FILE *infile = fopen(file_name, "rb");
    return read_file_stream(infile);
}

unsigned int load_dds(const char *path)
{
    unsigned int width, height, mip_map_count, block_size, format;
    unsigned char *data = (unsigned char *)read_file_binary(path);
    unsigned char *header = (unsigned char *)&(data[0]);
    unsigned char *buffer = (unsigned char *)&(data[128]);

    GLuint tid = 0;
    height = *(unsigned int *)&(header[12]);
    width = *(unsigned int *)&(header[16]);
    mip_map_count = (header[28]) | (header[29] << 8) | (header[30] << 16) | (header[31] << 24);

    if (header[84] == 'D')
    {
        switch (header[87])
        {
        case '1': // DXT1
            format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
            block_size = 8;
            break;
        case '3': // DXT3
            format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
            block_size = 16;
            break;
        case '5': // DXT5
            format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
            block_size = 16;
            break;
        default:
            break;
        }
    }
    glGenTextures(1, &tid);
    glBindTexture(GL_TEXTURE_2D, tid);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mip_map_count - 1); // opengl likes array length of mipmaps
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // don't forget to enable mipmaping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    unsigned int offset = 0;
    unsigned int size = 0;
    unsigned int w = width;
    unsigned int h = height;
    for (unsigned int i = 0; i < mip_map_count; i++)
    {
        if (w == 0 || h == 0)
        {
            mip_map_count--;
            continue;
        }
        size = ((w + 3) / 4) * ((h + 3) / 4) * block_size;
        glCompressedTexImage2D(GL_TEXTURE_2D, i, format, w, h, 0, size, buffer + offset);
        offset += size;
        w /= 2;
        h /= 2;
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mip_map_count - 1);
    glBindTexture(GL_TEXTURE_2D, 0);
    free(data);
    return tid;
}

GLFWwindow *create_window(int width, int height)
{
    if (!glfwInit())
    {
        return 0;
    }
    GLFWwindow *window = glfwCreateWindow(width, height, "CEngine", NULL, NULL);
    if (!window)
    {
        return 0;
    }
    glfwMakeContextCurrent(window);
    glewInit();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glfwSwapInterval(1); //vsync
    return window;
}

unsigned int create_shader(const char *code, unsigned int type)
{
    unsigned int shaderId = glCreateShader(type);
    glShaderSource(shaderId, 1, &code, NULL);
    glCompileShader(shaderId);

    GLint Result = GL_FALSE;
    int InfoLogLength;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0)
    {
        char *VertexShaderErrorMessage = malloc(sizeof(char) * InfoLogLength);
        glGetShaderInfoLog(shaderId, InfoLogLength, NULL, VertexShaderErrorMessage);
        printf("%s\n", &VertexShaderErrorMessage[0]);
    }
    return shaderId;
}

unsigned int create_shader_program(unsigned int fragment_shader, unsigned int vertex_shader)
{
    unsigned int program_id = glCreateProgram();
    glAttachShader(program_id, vertex_shader);
    glAttachShader(program_id, fragment_shader);

    glLinkProgram(program_id);
    GLint Result = GL_FALSE;
    int InfoLogLength;
    glGetProgramiv(program_id, GL_LINK_STATUS, &Result);
    glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0)
    {
        char *VertexShaderErrorMessage = malloc(sizeof(char) * InfoLogLength);
        glGetProgramInfoLog(program_id, InfoLogLength, NULL, VertexShaderErrorMessage);
        printf("%s\n", &VertexShaderErrorMessage[0]);
    }

    glDetachShader(program_id, fragment_shader);
    glDetachShader(program_id, vertex_shader);
    return program_id;
}

unsigned int create_shader_program_from_code(const char *vertex_code, const char *fragment_code)
{
    unsigned int vertexShader = create_shader(vertex_code, GL_VERTEX_SHADER);
    unsigned int fragmentShader = create_shader(fragment_code, GL_FRAGMENT_SHADER);
    unsigned int program_id = create_shader_program(vertexShader, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return program_id;
}

void uniform_matrix_4x4(unsigned int shader, const float *matrix, char *name)
{
    unsigned int MatrixID = glGetUniformLocation(shader, name);
    glUniformMatrix4fv(MatrixID, 1, GL_TRUE, matrix);
}

unsigned int create_vbo(const void *data, int size, int stride, int type)
{
    unsigned int vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    glEnableVertexAttribArray(type);
    glVertexAttribPointer(type, stride, GL_FLOAT, GL_FALSE, 0, NULL);
    return vertexbuffer;
}

void delete_gpu_mesh(unsigned int *mesh)
{
    for (int i = 0; i < MAX_VBOS; i++)
    {
        glDeleteBuffers(1, &mesh[i]);
    }
    glDeleteVertexArrays(1, &mesh[VAO]);
}

void draw_gpu_mesh(unsigned int *obj)
{
    glBindVertexArray(obj[VAO]);
    glDrawElements(
        GL_TRIANGLES,
        obj[NUM_INDICES],
        GL_UNSIGNED_INT,
        (void *)0);
}

Camera create_default_camera()
{
    return (Camera){
        .position = (float[]){0, 0, 0},
        .forward = (float[]){0, 0, -1},
        .aspect = 1,
        .nearClipPlane = 0.1f,
        .farClipPlane = 1000.0f,
        .fov = M_PI_2};
}

void load_mesh_to_gpu(unsigned int *out, Mesh mesh)
{
    glGenVertexArrays(1, &out[VAO]);
    glBindVertexArray(out[VAO]);
    out[VBO_VERTEX] = create_vbo(
        mesh.vertices,
        mesh.num_vertices * 3 * sizeof(float),
        3,
        VBO_VERTEX);

    out[VBO_NORMAL] = create_vbo(
        mesh.normals,
        mesh.num_vertices * 3 * sizeof(float),
        3,
        VBO_NORMAL);

    out[VBO_UV] = create_vbo(
        mesh.uvs,
        mesh.num_vertices * 2 * sizeof(float),
        2,
        VBO_UV);

    glGenBuffers(1, &out[VBO_INDICES]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, out[VBO_INDICES]);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        mesh.num_indices * 3 * sizeof(unsigned int),
        mesh.indices,
        GL_STATIC_DRAW);
    out[NUM_VERTICES] = mesh.num_vertices;
    out[NUM_INDICES] = mesh.num_indices * 3;
}

void draw_entities(Entity *entities, int count, float *view_projection_m, GLFWwindow *window)
{
    glfwPollEvents();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0, 0.2, 0.2, 1);
    float mvp[16] = {0};
    for (int i = 0; i < count; i++)
    {
        unsigned int *gpu_mesh = entities[i].gpu_mesh;
        glUseProgram(entities[i].shader);
        create_mvp(
            entities[i].position,
            entities[i].rotation,
            entities[i].scale,
            view_projection_m,
            mvp);
        uniform_matrix_4x4(entities[i].shader, mvp, "VP");
        GLuint TextureID = glGetUniformLocation(entities[i].shader, "myTextureSampler");
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, entities[i].texture);
        glUniform1i(TextureID, 0);
        draw_gpu_mesh(gpu_mesh);
    }
    glfwSwapBuffers(window);
}

Entity *load_entities_from_text(char *text, int *num_entities)
{
    char *token = strtok(text, "\n");
    int n = atoi(token);
    Entity *entities = malloc(sizeof(Entity) * n);
    (*num_entities) = n;

    int c = 0;
    while (token != NULL)
    {
        if (strcmp(token, "_entity_") == 0)
        {
            char *name = strtok(NULL, "\n");

            float *pos = malloc(3 * sizeof(float));
            pos[0] = atof(strtok(NULL, " "));
            pos[1] = atof(strtok(NULL, " "));
            pos[2] = atof(strtok(NULL, "\n"));

            float *rot = malloc(3 * sizeof(float));
            rot[0] = atof(strtok(NULL, " "));
            rot[1] = atof(strtok(NULL, " "));
            rot[2] = atof(strtok(NULL, "\n"));

            float *scale = malloc(3 * sizeof(float));
            scale[0] = atof(strtok(NULL, " "));
            scale[1] = atof(strtok(NULL, " "));
            scale[2] = atof(strtok(NULL, "\n"));

            char *mesh_file = strtok(NULL, "\n");
            char *texture_file = strtok(NULL, "\n");
            char *vshader = strtok(NULL, "\n");
            char *fshader = strtok(NULL, "\n");

            Mesh mesh = read_mesh(mesh_file);
            unsigned int texture = load_dds(texture_file);

            unsigned int *shaded_mesh = (unsigned int *)malloc(MAX_ATTRIBUTES * sizeof(unsigned int));
            load_mesh_to_gpu(shaded_mesh, mesh);
            unsigned int shader_program = create_shader_program_from_code(
                read_file(vshader),
                read_file(fshader));

            entities[c] = (Entity){
                .texture = texture,
                .name = name,
                .gpu_mesh = shaded_mesh,
                .position = pos,
                .rotation = rot,
                .scale = scale,
                .shader = shader_program};
            c++;
        }
        token = strtok(NULL, "\n");
    }
    return entities;
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
    v1[3] -= v2[3];
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
    cross_vector3(right, fwd, up);

    out[0] = right[0];
    out[1] = right[1];
    out[2] = right[2];
    out[3] = -dot_vector3(right, pos);

    out[4] = up[0];
    out[5] = up[1];
    out[6] = up[2];
    out[7] = -dot_vector3(up, pos);

    out[8] = fwd[0];
    out[9] = fwd[1];
    out[10] = fwd[2];
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
    set_4x4_matrix_scale(scale_matrix, 1.0f, 1.0f, 1.0f);
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

Mesh read_mesh(const char *file)
{
    char *buffer = read_file_binary(file);
    Mesh mesh;
    unsigned int offset = 0;
    mesh.num_vertices = *(unsigned int *)&(buffer[offset]);
    offset += 4;
    mesh.num_indices = *(unsigned int *)&(buffer[offset]);
    offset += 4;
    mesh.vertices = (float *)&(buffer[offset]);
    offset += mesh.num_vertices * 3 * 4;
    mesh.normals = (float *)&(buffer[offset]);
    offset += mesh.num_vertices * 3 * 4;
    mesh.uvs = (float *)&(buffer[offset]);
    offset += mesh.num_vertices * 2 * 4;
    mesh.indices = (unsigned int *)&(buffer[offset]);

    return mesh;
}