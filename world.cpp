#include "world.h"

World::World(Camera& wCam, InputHandler& wIh, Shader& wShader)
    : cam(wCam), ih(wIh), shader(wShader) {}

void World::clearScreen() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void World::addLightSource(glm::vec3 l) {
    lightSources.push_back(l);
}

void World::addCube(Cube c) {
    cubes.push_back(c);
}

void World::updateLights() {
    shader.installVec3A("lightSources", lightSources.data(), lightSources.size());
}

void World::draw() {
    for (auto c : cubes) {
        c.draw();
    }
}

void World::screenShot(char *filename) {
    char *buf = (char *)malloc(3 * cam.screenWidth * cam.screenHeight);
    glPixelStorei(GL_PACK_ALIGNMENT, 1); // avoids row padding
    glReadPixels(0, 0, cam.screenWidth, cam.screenHeight, GL_RGB, GL_UNSIGNED_BYTE, buf);
    stbi_flip_vertically_on_write(1); // flip before writing
    stbi_write_png(filename, cam.screenWidth, cam.screenHeight, 3, buf, cam.screenWidth * 3);
}
