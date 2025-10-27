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
