#pragma once
#include <SDL2/SDL.h>
#include "camera.h"

class InputHandler {
public:
    bool running;
    bool cameraUpdated;
    Camera & cam;

    InputHandler(Camera & cCam);

    void processInput();
    void handleMouseEvent(SDL_Event event);
    void handleKeyboardEvent(SDL_Event event);
};