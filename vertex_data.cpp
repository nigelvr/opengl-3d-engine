#include "vertex_data.h"

VertexData::VertexData(const char* vertexDataFilename) {
    vertexRawData = read_file_data(vertexDataFilename);

    num_vertices = 0;
    count_vertices();

    vertices = (float*)calloc(num_vertices, sizeof(float));
    read_vertices();

    initOpenglVertexObjects();
}

VertexData::~VertexData() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    free(vertices);
}

void VertexData::count_vertices() {
    for (int i = 0; i < strlen(vertexRawData); i++) {
        if (vertexRawData[i] == ',') {
            num_vertices++;
        }
    }
    num_vertices++;
}

void VertexData::read_vertices() {
    char buf[8];
    memset(buf, 0, sizeof(buf));
    int j = 0, k = 0;
    for (int i = 0; i <= strlen(vertexRawData); i++) {
        if (vertexRawData[i] == ',' || i == strlen(vertexRawData)) {
            vertices[k++] = atof(buf);
            memset(buf, 0, sizeof(buf));
            j = 0;
        } else {
            buf[j++] = vertexRawData[i];
        }
    }
}

void VertexData::initOpenglVertexObjects() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, num_vertices * sizeof(float), vertices, GL_STATIC_DRAW);

    // Position attribute (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Color attribute (location = 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
}
