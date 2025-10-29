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

// ------------------------------------------------------------
// WireCube
// ------------------------------------------------------------
WireCube::WireCube(glm::vec3 position,
                   glm::vec3 color,
                   glm::vec3 edgeColor,
                   float     edgeThickness)
    : color(color), edgeColor(edgeColor), edgeThickness(edgeThickness)
{
    model = glm::translate(glm::mat4(1.0f), position);
    setupMesh();
}

WireCube::~WireCube()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO_faces);
    glDeleteBuffers(1, &EBO_edges);
}

void WireCube::draw(std::shared_ptr<Shader> shader)
{
    shader->use();
    shader->installM4("model", model);
    shader->installVec3("objectColor", color);

    // --- Draw filled cube ---
    glBindVertexArray(VAO);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawElements(GL_TRIANGLES, numFaceIndices, GL_UNSIGNED_INT, 0);

    // --- Draw bold edges ---
    glLineWidth(edgeThickness);
    shader->installVec3("objectColor", edgeColor);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_edges);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawElements(GL_LINES, numEdgeIndices, GL_UNSIGNED_INT, 0);

    // reset state
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glLineWidth(1.0f);
    glBindVertexArray(0);
}

void WireCube::setupMesh()
{
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

    unsigned int faceIndices[] = {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4,
        0, 1, 5, 5, 4, 0,
        2, 3, 7, 7, 6, 2,
        0, 3, 7, 7, 4, 0,
        1, 2, 6, 6, 5, 1
    };

    unsigned int edgeIndices[] = {
        0, 1, 1, 2, 2, 3, 3, 0,  // back face
        4, 5, 5, 6, 6, 7, 7, 4,  // front face
        0, 4, 1, 5, 2, 6, 3, 7   // sides
    };

    numFaceIndices = sizeof(faceIndices) / sizeof(faceIndices[0]);
    numEdgeIndices = sizeof(edgeIndices) / sizeof(edgeIndices[0]);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO_faces);
    glGenBuffers(1, &EBO_edges);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_faces);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(faceIndices), faceIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_edges);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(edgeIndices), edgeIndices, GL_STATIC_DRAW);

    glBindVertexArray(0);
}