#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"
#include "vertex_data.h"

class Cube {
public:
    glm::vec3 pos;
    glm::vec3 color;
    float scale;
    VertexData& vertexData;
    Shader& shader;

    Cube(glm::vec3 cubePos, glm::vec3 cubeColor, float cubeScale, VertexData& vd, Shader& cubeShader);

    glm::mat4 model();
    void draw();
};

class Lamp : public Cube {
public:
    Lamp(glm::vec3 pos, VertexData& vd, Shader& shader);
    Lamp(glm::vec3 pos, glm::vec3 color, VertexData& vd, Shader& shader);

    void draw();
};
