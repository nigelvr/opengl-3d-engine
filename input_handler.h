#pragma once
#include <memory>
#include <SDL2/SDL.h>
#include "camera.h"

class InputHandler {
public:
    bool running;
    bool cameraUpdated;
    std::shared_ptr<Camera> camera;

    InputHandler(std::shared_ptr<Camera> cam);

    void processInput();
    void handleMouseEvent(SDL_Event event);
    void handleKeyboardEvent(SDL_Event event);
    void handleMouseMotion(const SDL_MouseMotionEvent& motion);
private:
    bool firstMouseMovement;
    int lastX;
    int lastY;
};