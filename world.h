#pragma once
#include <vector>
#include <memory>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <stb_image.h>
#include <stb_image_write.h>

// #include "objects.h"
#include "shader.h"
#include "camera.h"
#include "input_handler.h"

class World {
public:
    std::shared_ptr<Camera> camera;
    std::shared_ptr<InputHandler> ih;
    std::shared_ptr<Shader> shader;

    std::vector<glm::vec3> lightSources;
    // std::vector<Cube> cubes;


    World(std::string jsonPath);

    void clearScreen();
    void updateLights();
    void draw();
    void screenShot(char *filename);
    void addLightSource(glm::vec3 lightSource);
};
