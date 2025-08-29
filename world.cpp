#include "world.h"

World::World(Camera& wCam, InputHandler& wIh, Shader& wShader, Lamp* wLamps, int wNumLamps, Cube* wCubes, int wNumCubes)
    : cam(wCam), ih(wIh), shader(wShader), lamps(wLamps), numLamps(wNumLamps), cubes(wCubes), numCubes(wNumCubes) {}

void World::clearScreen() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void World::updateLights() {
    glm::vec3* lightPositions = new glm::vec3[numLamps];
    for (int i = 0; i < numLamps; i++) {
        lightPositions[i] = lamps[i].pos;
    }
    shader.installVec3A("lightSources", lightPositions, numLamps);
    delete[] lightPositions;  // FIXED: must use delete[] for array
}

void World::draw() {
    for (int i = 0; i < numLamps; i++) {
        lamps[i].draw();
    }
    for (int i = 0; i < numCubes; i++) {
        cubes[i].draw();
    }
}
