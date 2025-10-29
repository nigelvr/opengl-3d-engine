#include "world.h"

World::World(std::string jsonPath) {
    camera = std::make_shared<Camera>(jsonPath);
    ih = std::make_shared<InputHandler>(camera);
    shader = std::make_shared<LightingShader>();
}

void World::clearScreen() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void World::addLightSource(glm::vec3 l) {
    lightSources.push_back(l);
}

/* void World::addCube(Cube c) {
    cubes.push_back(c);
} */

void World::updateLights() {
    shader->installVec3A("lightSources", lightSources.data(), lightSources.size());
}

/* void World::draw() {
    for (auto c : cubes) {
        c.draw(shader);
    }
} */

void World::screenShot(char *filename) {
    char *buf = (char *)malloc(3 * camera->screenWidth * camera->screenHeight);
    glPixelStorei(GL_PACK_ALIGNMENT, 1); // avoids row padding
    glReadPixels(0, 0, camera->screenWidth, camera->screenHeight, GL_RGB, GL_UNSIGNED_BYTE, buf);
    stbi_flip_vertically_on_write(1); // flip before writing
    stbi_write_png(filename, camera->screenWidth, camera->screenHeight, 3, buf, camera->screenWidth * 3);
}
