#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <cstddef>      // for std::size_t (used by GLsizei)

#include "shader.h"
// #include "vertex_data.h"   // (not used in the original file)

class Shader;           // forward declaration – enough for std::shared_ptr<Shader>

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