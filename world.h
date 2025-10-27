#pragma once
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <stb_image.h>
#include <stb_image_write.h>

#include "objects.h"
#include "shader.h"
#include "camera.h"
#include "input_handler.h"

class World {
public:
    Camera& cam;
    InputHandler& ih;
    Shader& shader;
    std::vector<glm::vec3> lightSources;
    std::vector<Cube> cubes;


    World(Camera& wCam, InputHandler& wIh, Shader& wShader);

    void clearScreen();
    void updateLights();
    void draw();
    void screenShot(char *filename);
    void addLightSource(glm::vec3 lightSource);
    void addCube(Cube c);
};
