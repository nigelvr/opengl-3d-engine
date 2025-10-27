#include <stdio.h>
#include <utility>
#include <string>

#include <SDL2/SDL.h>

// #include <SDL2/SDL_hints.h>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stb_image.h>

#include "common.h"
#include "camera.h"
#include "input_handler.h"
#include "shader.h"
#include "vertex_data.h"
#include "texture.h"
#include "objects.h"
#include "world.h"

#define SCR_WIDTH 800
#define SCR_HEIGHT 600


int main(int argc, char* argv[]) {
    bool debug = false;
    bool screenshot = false;
    for (int i = 0; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-s" || arg == "--screenshot") {
            screenshot = true;
        } else if (arg == "-d" || arg == "--debug") {
            debug = true;
        }
    }

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("OpenGL Triangle",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCR_WIDTH, SCR_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

    SDL_GLContext context = SDL_GL_CreateContext(window);
    glewExperimental = GL_TRUE;
    glewInit();

    glEnable(GL_DEPTH_TEST);

    // === Define Triangle Vertex Data ===
    VertexData vertexData("./cube.data");
    LightingShader shader;

    // Camera
    Camera camera("config.json");
    
    shader.installVec3("cameraPos", camera.cameraPos);

    // compute view, proj and model matrices
    // initialize model to be the identy matrix. update in the loop
    glm::mat4 view = camera.viewMatrix();
    glm::mat4 projection = camera.projectionMatrix();
    glm::mat4 model = glm::mat4(1.0f);
    shader.installM4("view", view);
    shader.installM4("projection", projection);
    shader.installM4("model", model);

    // light and box color
    auto objectColor = glm::vec3(1.0f, 0.5f, 0.31f);
    auto lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 lightPos(0.0f, 0.0f, 1.0f);
    shader.installVec3("objectColor", objectColor);
    shader.installVec3("lightColor", lightColor);
    shader.installVec3("lightPos", lightPos);

    // our world
    Cube cubes[] = {
        Cube(glm::vec3( 0.0f,  0.0f, 0.0f), glm::vec3(1.0f, 0.5f, 0.31f), 1.0f, vertexData, shader),
    };
    int numCubes = sizeof(cubes)/sizeof(cubes[0]);
    glm::vec3 lightSources[] = {
        glm::vec3(0.0f, -1.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, -1.0f, 0.0f),
    };
    shader.installInt("numLights", sizeof(lightSources)/sizeof(lightSources[0]));

    // input handler
    InputHandler ih(camera);
    World world(camera, ih, shader);
    for (auto l : lightSources) {
        world.addLightSource(l);
    }
    for (auto c : cubes) {
        world.addCube(c);
    }

    // === Render Loop ===
    int curTick, lastTick=0, deltaTick;
    double deltaTime = 0.1f;

    while (ih.running) {
        int mousex, mousey;
        SDL_GetMouseState(&mousex, &mousey);
        if (debug) {
            printf("cam = %f %f %f ; yaw = %f ; mouse = %d %d\n", camera.cameraPos.x, camera.cameraPos.y, camera.cameraPos.z, camera.yaw, mousex, mousey);
        }
        curTick = SDL_GetTicks();
        // set cam speed
        deltaTick = curTick-lastTick;
        lastTick = curTick;
        deltaTime = (float)deltaTick/1000.0f;
        camera.setCamSpeed(deltaTime);

        // get keyboard + mouse input
        ih.processInput();
        if (ih.cameraUpdated) {
            if (screenshot) {
                char filename[256];
                memset(filename, 0, 256);
                sprintf(filename, "png/%f_%f_%f_%f_%f_%f.png", camera.cameraPos.x, camera.cameraPos.y, camera.cameraPos.z, camera.yaw, camera.pitch, curTick);
                world.screenShot(filename);
            }
            view = camera.viewMatrix();
            shader.installM4("view", view);
            shader.installVec3("cameraPos", camera.cameraPos);
            ih.cameraUpdated = false;
        }

        // clear & draw
        world.clearScreen();
        world.updateLights();
        world.draw();

        SDL_GL_SwapWindow(window);
        deltaTime = glfwGetTime() - deltaTime;
    }

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
