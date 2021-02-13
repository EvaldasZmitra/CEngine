#include "graphics.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "util.h"
#include <string.h>

GLFWwindow *create_window(int width, int height)
{
    glfwSetErrorCallback(error_callback);
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

void error_callback(int error, const char *description)
{
    fprintf(stderr, "Error: %s\n", description);
}

unsigned int create_shader(const char *code, unsigned int type)
{
    unsigned int shaderId = glCreateShader(type);
    glShaderSource(shaderId, 1, &code, NULL);
    glCompileShader(shaderId);
    return shaderId;
}

unsigned int create_shader_program(unsigned int fragment_shader, unsigned int vertex_shader)
{
    unsigned int program_id = glCreateProgram();
    glAttachShader(program_id, vertex_shader);
    glAttachShader(program_id, fragment_shader);
    glLinkProgram(program_id);
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

unsigned int create_vbo(const float *data, int size, int stride, int type)
{
    unsigned int vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    glEnableVertexAttribArray(type);
    glVertexAttribPointer(type, stride, GL_FLOAT, GL_FALSE, 0, NULL);
    return vertexbuffer;
}

void create_gpu_mesh(const float *vertices, int num_vertices, unsigned int *out)
{
    glGenVertexArrays(1, &out[VAO]);
    glBindVertexArray(out[VAO]);
    out[VBO_VERTEX] = create_vbo(
        vertices,
        num_vertices * 3 * sizeof(float),
        3,
        VBO_VERTEX);
    out[NUM_VERTICES] = num_vertices;
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
    glDrawArrays(GL_TRIANGLES, 0, obj[NUM_VERTICES]);
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

void load_shaded_mesh(unsigned int *out, char *vertex_shader_code, char *fragment_shader_code)
{
    GLuint shader_program = create_shader_program_from_code(
        vertex_shader_code,
        fragment_shader_code);
    GLfloat vertices[] = {
        -1.0f, -1.0f, -1.0f, // triangle 1 : begin
        -1.0f, -1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f, // triangle 1 : end
        1.0f, 1.0f, -1.0f, // triangle 2 : begin
        -1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f, // triangle 2 : end
        1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f};
    out[SHADER] = shader_program;
    unsigned int num_vertices = sizeof(vertices) / sizeof(vertices[0]) / 3;
    create_gpu_mesh(vertices, num_vertices, out);
    free(vertices);
}

void draw_entities(Entity *entities, int count, float *view_projection_m, GLFWwindow *window)
{
    glfwPollEvents();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    float mvp[16] = {0};
    for (int i = 0; i < count; i++)
    {
        unsigned int *gpu_mesh = entities[i].gpu_mesh;
        glUseProgram(gpu_mesh[SHADER]);
        create_mvp(
            entities[i].position,
            entities[i].rotation,
            entities[i].scale,
            view_projection_m,
            mvp);
        uniform_matrix_4x4(gpu_mesh[SHADER], mvp, "VP");
        draw_gpu_mesh(gpu_mesh);
    }
    glfwSwapBuffers(window);
}

void load_vertices(char *file)
{
}

unsigned int load_dds_to_gpu(
    unsigned char *buffer,
    unsigned int format,
    unsigned int mip_map_count,
    unsigned int width,
    unsigned int height)
{
    switch (format)
    {
    case 0x31545844:
        format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
        break;
    case 0x33545844:
        format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
        break;
    case 0x35545844:
        format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
        break;
    default:
        break;
    }

    unsigned int texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
    unsigned int offset = 0;
    for (unsigned int i = 0; i < mip_map_count; i++)
    {
        unsigned int size = ((width + 3) / 4) * ((height + 3) / 4) * blockSize;
        glCompressedTexImage2D(
            GL_TEXTURE_2D,
            i,
            format,
            width,
            height,
            0,
            size,
            buffer + offset);
        offset += size;
        width /= 2;
        height /= 2;
    }
    return texture_id;
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

            char *mesh = strtok(NULL, "\n");
            char *vshader = strtok(NULL, "\n");
            char *fshader = strtok(NULL, "\n");

            load_vertices(mesh);

            unsigned int *shaded_mesh = malloc(MAX_ATTRIBUTES * sizeof(int));
            load_shaded_mesh(
                shaded_mesh,
                read_file(vshader),
                read_file(fshader));
            entities[c] = (Entity){
                .name = name,
                .gpu_mesh = shaded_mesh,
                .position = pos,
                .rotation = rot,
                .scale = scale};
            c++;
        }
        token = strtok(NULL, "\n");
    }
    return entities;
}
