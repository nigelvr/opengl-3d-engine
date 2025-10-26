#include "world.h"

World::World(Camera& wCam, InputHandler& wIh, Shader& wShader)
    : cam(wCam), ih(wIh), shader(wShader) {}

void World::clearScreen() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void World::addLamp(Lamp l) {
    lamps.push_back(l);
}

void World::addCube(Cube c) {
    cubes.push_back(c);
}

void World::updateLights() {
    int numLamps = lamps.size();
    glm::vec3* lightPositions = new glm::vec3[numLamps];
    for (int i = 0; i < numLamps; i++) {
        lightPositions[i] = lamps[i].pos;
    }
    shader.installVec3A("lightSources", lightPositions, numLamps);
    delete[] lightPositions;  // FIXED: must use delete[] for array
}

void World::draw() {
    for (auto l : lamps) {
        l.draw();
    }
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
