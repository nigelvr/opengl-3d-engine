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

class CubeX : public Renderable {
public:
    CubeX(glm::vec3 position = glm::vec3(0.0f),
          glm::vec3 color    = glm::vec3(1.0f));

    ~CubeX() override;

private:
    GLuint VBO = 0, EBO = 0;

    void setupMesh();
};

class WireCube : public Renderable {
public:
    WireCube(glm::vec3 position      = glm::vec3(0.0f),
             glm::vec3 color         = glm::vec3(1.0f),
             glm::vec3 edgeColor     = glm::vec3(0.0f),
             float     edgeThickness = 5.0f);

    ~WireCube() override;

    void draw(std::shared_ptr<Shader> shader) override;

private:
    GLuint  VAO = 0, VBO = 0, EBO_faces = 0, EBO_edges = 0;
    GLsizei numFaceIndices = 0, numEdgeIndices = 0;
    glm::vec3 color;
    glm::vec3 edgeColor;
    float     edgeThickness;

    void setupMesh();
};