#pragma once
#include <GL/glew.h>
#include <cstring>
#include <cstdlib>
#include "common.h"

class VertexData {
public:
    float* vertices;
    int num_vertices;
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;

    VertexData(const char* vertexDataFilename);
    ~VertexData();

private:
    const char* vertexRawData;

    void count_vertices();
    void read_vertices();
    void initOpenglVertexObjects();
};
