#include <iostream>

#include "renderable.h"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"

// ------------------------------------------------------------
// Renderable
// ------------------------------------------------------------
void Renderable::draw(std::shared_ptr<Shader> shader)
{
    shader->installM4("model", model);
    shader->installVec3("objectColor", color);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

// ------------------------------------------------------------
// CubeX
// ------------------------------------------------------------
CubeX::CubeX(glm::vec3 position, glm::vec3 color)
{
    this->color = color;
    this->model = glm::translate(glm::mat4(1.0f), position);

    setupMesh();
}

CubeX::~CubeX()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void CubeX::setupMesh()
{
    // Basic cube (positions only)
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f
    };

    unsigned int indices[] = {
        0, 1, 2, 2, 3, 0,   // back
        4, 5, 6, 6, 7, 4,   // front
        0, 1, 5, 5, 4, 0,   // bottom
        2, 3, 7, 7, 6, 2,   // top
        0, 3, 7, 7, 4, 0,   // left
        1, 2, 6, 6, 5, 1    // right
    };

    numIndices = sizeof(indices) / sizeof(indices[0]);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

WireCube::WireCube(glm::vec3 position,
                   glm::vec3 color,
                   glm::vec3 edgeColor,
                   float edgeThickness)
    : color(color),
      edgeColor(edgeColor),
      edgeThickness(edgeThickness)
{
    model = glm::translate(glm::mat4(1.0f), position);
    setupMesh();
}

WireCube::~WireCube() {
    glDeleteVertexArrays(1, &VAO_faces);
    glDeleteVertexArrays(1, &VAO_edges);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO_faces);
    glDeleteBuffers(1, &EBO_edges);
}

void WireCube::draw(std::shared_ptr<Shader> shader) {
    shader->use();
    shader->installM4("model", model);

    // --- Draw solid cube ---
    shader->installVec3("objectColor", color);
    glBindVertexArray(VAO_faces);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawElements(GL_TRIANGLES, numFaceIndices, GL_UNSIGNED_INT, 0);

    // --- Draw bold edges ---
    shader->installVec3("objectColor", edgeColor);
    glBindVertexArray(VAO_edges);
    glLineWidth(edgeThickness);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawElements(GL_LINES, numEdgeIndices, GL_UNSIGNED_INT, 0);

    // reset state
    glBindVertexArray(0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glLineWidth(1.0f);
}

void WireCube::setupMesh() {
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  // 0
         0.5f, -0.5f, -0.5f,  // 1
         0.5f,  0.5f, -0.5f,  // 2
        -0.5f,  0.5f, -0.5f,  // 3
        -0.5f, -0.5f,  0.5f,  // 4
         0.5f, -0.5f,  0.5f,  // 5
         0.5f,  0.5f,  0.5f,  // 6
        -0.5f,  0.5f,  0.5f   // 7
    };

    unsigned int faceIndices[] = {
        0, 1, 2, 2, 3, 0,   // back
        4, 7, 6, 6, 5, 4,   // front
        0, 4, 5, 5, 1, 0,   // bottom
        3, 2, 6, 6, 7, 3,   // top
        0, 3, 7, 7, 4, 0,   // left
        1, 5, 6, 6, 2, 1    // right
    };

    unsigned int edgeIndices[] = {
        0, 1, 1, 2, 2, 3, 3, 0,  // back
        4, 5, 5, 6, 6, 7, 7, 4,  // front
        0, 4, 1, 5, 2, 6, 3, 7   // connectors
    };

    numFaceIndices = sizeof(faceIndices) / sizeof(faceIndices[0]);
    numEdgeIndices = sizeof(edgeIndices) / sizeof(edgeIndices[0]);

    // Shared vertex buffer
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // --- VAO for faces ---
    glGenVertexArrays(1, &VAO_faces);
    glGenBuffers(1, &EBO_faces);

    glBindVertexArray(VAO_faces);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_faces);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(faceIndices), faceIndices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // --- VAO for edges ---
    glGenVertexArrays(1, &VAO_edges);
    glGenBuffers(1, &EBO_edges);

    glBindVertexArray(VAO_edges);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_edges);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(edgeIndices), edgeIndices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
