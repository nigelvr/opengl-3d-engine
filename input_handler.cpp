#include "input_handler.h"
#include <iostream>
#include <SDL2/SDL.h>

InputHandler::InputHandler(std::shared_ptr<Camera> cam)
    : camera(cam), running(true), cameraUpdated(false), firstMouseMovement(true), lastX(0), lastY(0)
{}

void InputHandler::processInput()
{
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        switch (ev.type) {
            case SDL_QUIT:               running = false; break;
            case SDL_KEYDOWN:            handleKeyboardEvent(ev); break;
            case SDL_MOUSEMOTION:        handleMouseMotion(ev.motion); break;
        }
    }

    // ---- continuous keyboard movement (hold W/A/S/D) ----
    const Uint8* keys = SDL_GetKeyboardState(nullptr);
    if (keys[SDL_SCANCODE_W]) { camera->moveForward();  cameraUpdated = true; }
    if (keys[SDL_SCANCODE_S]) { camera->moveBackward(); cameraUpdated = true; }
    if (keys[SDL_SCANCODE_A]) { camera->moveLeft();     cameraUpdated = true; }
    if (keys[SDL_SCANCODE_D]) { camera->moveRight();    cameraUpdated = true; }
}

void InputHandler::handleMouseMotion(const SDL_MouseMotionEvent& motion)
{
    if (firstMouseMovement) {
        lastX = motion.x;
        lastY = motion.y;
        firstMouseMovement = false;

        // centre the cursor once
        SDL_WarpMouseInWindow(nullptr,
            camera->screenWidth / 2,
            camera->screenHeight / 2);
        SDL_ShowCursor(SDL_DISABLE);
        SDL_SetRelativeMouseMode(SDL_TRUE);
        return;
    }

    float xoffset = motion.x - lastX;
    float yoffset = lastY - motion.y;   // inverted Y

    lastX = motion.x;
    lastY = motion.y;

    camera->processMouseMovement(xoffset, yoffset, true);
    cameraUpdated = true;
}





void InputHandler::handleKeyboardEvent(SDL_Event event) {
    switch (event.key.keysym.sym) {
        case SDLK_ESCAPE:
            running = false;
            break;
        case SDLK_LEFT:
            cameraUpdated = true;
            camera->yaw -= 1;
            break;
        case SDLK_RIGHT:
            cameraUpdated = true;
            camera->yaw += 1;
            break;
        case SDLK_DOWN:
            cameraUpdated = true;
            camera->cameraPos.y -= 1;
            break;
        case SDLK_UP:
            cameraUpdated = true;
            camera->cameraPos.y -= 1;
            break;
        
    }
}
