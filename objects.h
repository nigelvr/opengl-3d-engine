#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"
#include "vertex_data.h"

/* class Renderable {
public:
    VertexData &vertexData;

    Renderable(VertexData &vd);
    virtual ~Renderable();
    virtual glm::mat4 model();
    virtual void draw(Shader &shader);
}; */

class Cube {
public:
    glm::vec3 pos;
    glm::vec3 color;
    float scale;
    VertexData& vertexData;

    Cube(glm::vec3 cubePos, glm::vec3 cubeColor, float cubeScale, VertexData& vd);

    glm::mat4 model();
    void draw(Shader &shader);
};
