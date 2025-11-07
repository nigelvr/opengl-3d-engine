#include <stdio.h>
#include <utility>
#include <string>
#include <filesystem>

#include <SDL2/SDL.h>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "common.h"
#include "camera.h"
#include "input_handler.h"
#include "shader.h"
#include "renderable.h"
#include "world.h"

#define SCR_WIDTH 800
#define SCR_HEIGHT 600


int main(int argc, char* argv[]) {
    bool debug = false;
    bool screenshot = false;
    bool fullscreen = false;
    std::string objdirpath = "./assets/models/cube";
    for (int i = 0; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-s" || arg == "--screenshot") {
            screenshot = true;
        } else if (arg == "-d" || arg == "--debug") {
            debug = true;
        } else if (arg == "--data") {
            assert(i+1 < argc);
            objdirpath = argv[i+1];
        } else if (arg == "-f" || arg == "--fullscreen") {
            fullscreen = true;
        }
    }

    SDL_Init(SDL_INIT_VIDEO);
    uint32_t window_flag = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
    if (fullscreen) {
        window_flag |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    }
    SDL_Window* window = SDL_CreateWindow("OpenGL Engine",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCR_WIDTH, SCR_HEIGHT, window_flag);
    //SDL_ShowCursor(SDL_DISABLE);
    SDL_SetRelativeMouseMode(SDL_TRUE);   // Line 104
    SDL_GLContext context = SDL_GL_CreateContext(window);
    glewExperimental = GL_TRUE;
    glewInit();
    glEnable(GL_DEPTH_TEST);

    glm::vec3 lightSources[] = {
        glm::vec3(0.0f, -1.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
    };
    
    World world("config.json");
    for (auto l : lightSources) {
        world.addLightSource(l);
    }

    // shader variables
    glm::mat4 view = world.camera->viewMatrix();
    glm::mat4 projection = world.camera->projectionMatrix();
    glm::mat4 model = glm::mat4(1.0f);
    auto objectColor = glm::vec3(0.0f, 0.5f, 1.0f);
    auto lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 lightPos(0.0f, 0.0f, 1.0f);

    // install all the shader variables
    world.shader->installVec3("cameraPos", world.camera->cameraPos);
    world.shader->installM4("view", view);
    world.shader->installM4("projection", projection);
    world.shader->installM4("model", model);
    world.shader->installVec3("objectColor", objectColor);
    world.shader->installVec3("lightColor", lightColor);
    world.shader->installInt("numLights", sizeof(lightSources)/sizeof(lightSources[0]));
    world.shader->installBool("useTexture", true);

    std::filesystem::path fs_objfilepath(objdirpath);
    auto objfilepath = objdirpath + "/" + fs_objfilepath.filename().string() + ".obj";
    auto obj = Mesh(objfilepath, glm::vec3( 0.0f,  0.0f, 0.0f), glm::vec3(0.0f, 0.5f, 1.0f));

    // === Render Loop ===
    int curTick, lastTick=0, deltaTick;
    double deltaTime = 0.1f;

    SDL_WarpMouseInWindow(window, 257, 562);
    while (world.ih->running) {
        int mousex, mousey;
        SDL_GetMouseState(&mousex, &mousey);
        if (debug) {
            printf("cam = %f %f %f ; yaw = %f ; pitch = %f ; mouse = %d %d\n",
                world.camera->cameraPos.x,
                world.camera->cameraPos.y,
                world.camera->cameraPos.z,
                world.camera->yaw,
                world.camera->pitch,
                mousex,
                mousey
            );
        }
        curTick = SDL_GetTicks();
        // set cam speed
        deltaTick = curTick-lastTick;
        lastTick = curTick;
        deltaTime = (float)deltaTick/1000.0f;
        world.camera->setCamSpeed(deltaTime);

        // get keyboard + mouse input
        world.ih->processInput();
        if (world.ih->cameraUpdated) {
            if (screenshot) {
                char filename[256];
                memset(filename, 0, 256);
                sprintf(filename, "png/%f_%f_%f_%f_%f_%f.png", world.camera->cameraPos.x, world.camera->cameraPos.y, world.camera->cameraPos.z, world.camera->yaw, world.camera->pitch, curTick);
                world.screenShot(filename);
            }
            view = world.camera->viewMatrix();
            world.shader->installM4("view", view);
            world.shader->installVec3("cameraPos", world.camera->cameraPos);
            world.ih->cameraUpdated = false;
        }

        // clear & draw
        world.clearScreen();
        world.updateLights();
        // world.draw();
        obj.draw(world.shader, false);

        SDL_GL_SwapWindow(window);
        deltaTime = glfwGetTime() - deltaTime;
    }

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
