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
    unsigned int VAO, VBO, EBO;
    unsigned int VAO_edges, EBO_edges; // for wireframe
    unsigned int numVertexIndices = 0;
    unsigned int numEdgeIndices = 0;

    virtual void draw(std::shared_ptr<Shader> shader, bool wireFrame);
    virtual ~Renderable() = default;
};

class Mesh : public Renderable {
public:
    Mesh(std::string objfile, glm::vec3 position, glm::vec3 color);
    ~Mesh() override;
};