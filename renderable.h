#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <cstddef>

#include "shader.h"

class Shader;

struct Vertex {
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 uv;

    Vertex(glm::vec3 v, glm::vec3 n, glm::vec2 u) {
        this->pos = v;
        this->normal = n;
        this->uv = u;
    }
};

class Renderable {
public:
    glm::mat4 model;
    glm::vec3 color;
    GLuint    VAO = 0;
    GLsizei   numIndices = 0;

    virtual void draw(std::shared_ptr<Shader> shader);
    virtual ~Renderable() = default;
};

class SimpleCube : public Renderable {
public:
    SimpleCube(glm::vec3 position = glm::vec3(0.0f),
          glm::vec3 color    = glm::vec3(1.0f));

    ~SimpleCube() override;

private:
    GLuint VBO = 0, EBO = 0;

    void setupMesh();
};

class WireCube : public Renderable {
public:
    glm::vec3 color;
    glm::vec3 edgeColor;
    float edgeThickness;

    WireCube(glm::vec3 position,
             glm::vec3 color,
             glm::vec3 edgeColor,
             float edgeThickness);

    ~WireCube();

    void draw(std::shared_ptr<Shader> shader) override;

private:
    unsigned int VAO_faces, VAO_edges;
    unsigned int VBO, EBO_faces, EBO_edges;
    int numFaceIndices, numEdgeIndices;

    void setupMesh();
};