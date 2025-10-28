#include "objects.h"

// ------------------- Cube -------------------

Cube::Cube(glm::vec3 cubePos, glm::vec3 cubeColor, float cubeScale, VertexData& vd)
    : pos(cubePos), color(cubeColor), vertexData(vd), scale(cubeScale) {}

glm::mat4 Cube::model() {
    auto m = glm::mat4(1.0f);
    m = glm::translate(m, pos);
    m = glm::scale(m, glm::vec3(scale));
    return m;
}

void Cube::draw(std::shared_ptr<Shader> shader) {
    shader->installM4("model", model());
    shader->installVec3("objectColor", color);
    shader->installBool("isLamp", false);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}
