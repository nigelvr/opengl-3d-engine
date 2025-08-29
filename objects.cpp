#include "objects.h"

// ------------------- Cube -------------------

Cube::Cube(glm::vec3 cubePos, glm::vec3 cubeColor, float cubeScale, VertexData& vd, Shader& cubeShader)
    : pos(cubePos), color(cubeColor), vertexData(vd), shader(cubeShader), scale(cubeScale) {}

glm::mat4 Cube::model() {
    auto m = glm::mat4(1.0f);
    m = glm::translate(m, pos);
    m = glm::scale(m, glm::vec3(scale));
    return m;
}

void Cube::draw() {
    shader.installM4("model", model());
    shader.installVec3("objectColor", color);
    shader.installBool("isLamp", false);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

// ------------------- Lamp -------------------

Lamp::Lamp(glm::vec3 pos, VertexData& vd, Shader& shader)
    : Cube(pos, glm::vec3(1.0f, 1.0f, 1.0f), 0.2f, vd, shader) {}

Lamp::Lamp(glm::vec3 pos, glm::vec3 color, VertexData& vd, Shader& shader)
    : Cube(pos, color, 0.2f, vd, shader) {}

void Lamp::draw() {
    shader.installM4("model", model());
    shader.installVec3("lightPos", pos);
    shader.installVec3("lightColor", color);
    shader.installBool("isLamp", true);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}
