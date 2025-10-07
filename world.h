#pragma once
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
    Lamp* lamps;
    int numLamps;
    Cube* cubes;
    int numCubes;

    World(Camera& wCam, InputHandler& wIh, Shader& wShader, Lamp* wLamps, int wNumLamps, Cube* wCubes, int wNumCubes);

    void clearScreen();
    void updateLights();
    void draw();
    void screenShot(char *filename);
};
