#include "graphics.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

GLuint loadBMP_custom(const char *imagepath)
{

    printf("Reading image %s\n", imagepath);
    unsigned char header[54];
    unsigned int dataPos;
    unsigned int imageSize;
    unsigned int width, height;
    unsigned char *data;
    FILE *file = fopen(imagepath, "rb");
    if (!file)
    {
        printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", imagepath);
        getchar();
        return 0;
    }
    if (fread(header, 1, 54, file) != 54)
    {
        printf("Not a correct BMP file\n");
        fclose(file);
        return 0;
    }
    if (header[0] != 'B' || header[1] != 'M')
    {
        printf("Not a correct BMP file\n");
        fclose(file);
        return 0;
    }
    if (*(int *)&(header[0x1E]) != 0)
    {
        printf("Not a correct BMP file\n");
        fclose(file);
        return 0;
    }
    if (*(int *)&(header[0x1C]) != 24)
    {
        printf("Not a correct BMP file\n");
        fclose(file);
        return 0;
    }
    dataPos = *(int *)&(header[0x0A]);
    imageSize = *(int *)&(header[0x22]);
    width = *(int *)&(header[0x12]);
    height = *(int *)&(header[0x16]);
    if (imageSize == 0)
        imageSize = width * height * 3;
    if (dataPos == 0)
        dataPos = 54;
    data = malloc(sizeof(unsigned char) * imageSize);
    fread(data, 1, imageSize, file);
    fclose(file);
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
    return textureID;
}

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

GLuint loadDDS2(const char *path)
{
    // lay out variables to be used
    unsigned char *header;

    unsigned int width;
    unsigned int height;
    unsigned int mipMapCount;

    unsigned int blockSize;
    unsigned int format;

    unsigned int w;
    unsigned int h;

    unsigned char *buffer = 0;
    GLuint tid = 0;
    FILE *f;
    if ((f = fopen(path, "rb")) == 0)
        return 0;
    fseek(f, 0, SEEK_END);
    long file_size = ftell(f);
    fseek(f, 0, SEEK_SET);
    header = malloc(128);
    fread(header, 1, 128, f);
    if (memcmp(header, "DDS ", 4) != 0)
    {
    }

    height = *(unsigned int *)&(header[12]);
    width = *(unsigned int *)&(header[16]);
    mipMapCount = (header[28]) | (header[29] << 8) | (header[30] << 16) | (header[31] << 24);

    if (header[84] == 'D')
    {
        switch (header[87])
        {
        case '1': // DXT1
            format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
            blockSize = 8;
            break;
        case '3': // DXT3
            format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
            blockSize = 16;
            break;
        case '5': // DXT5
            format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
            blockSize = 16;
            break;
        default:
            break;
        }
    }
    buffer = malloc(file_size - 128);
    fread(buffer, 1, file_size, f);
    glGenTextures(1, &tid);
    glBindTexture(GL_TEXTURE_2D, tid);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipMapCount - 1); // opengl likes array length of mipmaps
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // don't forget to enable mipmaping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    unsigned int offset = 0;
    unsigned int size = 0;
    w = width;
    h = height;
    for (unsigned int i = 0; i < mipMapCount; i++)
    {
        if (w == 0 || h == 0)
        { // discard any odd mipmaps 0x1 0x2 resolutions
            mipMapCount--;
            continue;
        }
        size = ((w + 3) / 4) * ((h + 3) / 4) * blockSize;
        glCompressedTexImage2D(GL_TEXTURE_2D, i, format, w, h, 0, size, buffer + offset);
        offset += size;
        w /= 2;
        h /= 2;
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipMapCount - 1);
    glBindTexture(GL_TEXTURE_2D, 0);
    free(buffer);
    free(header);
    fclose(f);
    return tid;
}

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

void load_shaded_mesh(unsigned int *out, Mesh mesh, char *vertex_shader_code, char *fragment_shader_code)
{
    GLuint shader_program = create_shader_program_from_code(
        vertex_shader_code,
        fragment_shader_code);
    out[SHADER] = shader_program;

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
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.num_indices * 3 * sizeof(unsigned int), mesh.indices, GL_STATIC_DRAW);
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
        glUseProgram(gpu_mesh[SHADER]);
        create_mvp(
            entities[i].position,
            entities[i].rotation,
            entities[i].scale,
            view_projection_m,
            mvp);
        uniform_matrix_4x4(gpu_mesh[SHADER], mvp, "VP");
        GLuint TextureID = glGetUniformLocation(gpu_mesh[SHADER], "myTextureSampler");
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, entities[i].texture);
        glUniform1i(TextureID, 0);

        draw_gpu_mesh(gpu_mesh);
    }
    glfwSwapBuffers(window);
}

unsigned int load_dds_to_gpu(
    unsigned char *buffer,
    unsigned int format,
    unsigned int mip_map_count,
    unsigned int width,
    unsigned int height)
{

    return loadDDS2("brick.DDS");
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
            unsigned int format, width, height, linear_size, mip_map_count, buffer_size;
            unsigned char *buffer = load_dds(
                texture_file,
                &width,
                &height,
                &linear_size,
                &mip_map_count,
                &format,
                &buffer_size);
            unsigned int texture = load_dds_to_gpu(buffer, format, mip_map_count, width, height);

            unsigned int *shaded_mesh = (unsigned int *)malloc(MAX_ATTRIBUTES * sizeof(unsigned int));
            load_shaded_mesh(
                shaded_mesh,
                mesh,
                read_file(vshader),
                read_file(fshader));
            entities[c] = (Entity){
                .texture = texture,
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
