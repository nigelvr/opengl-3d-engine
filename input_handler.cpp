#include "input_handler.h"
#include <iostream>
#include <SDL2/SDL.h>

InputHandler::InputHandler(std::shared_ptr<Camera> cam)
    : camera(cam), running(true), cameraUpdated(false)
{}

void InputHandler::processInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        } else if (event.type == SDL_KEYDOWN) {
            handleKeyboardEvent(event);
        } else if (event.type == SDL_MOUSEMOTION) {
            handleMouseEvent(event);
        }
    }
}

void InputHandler::handleMouseEvent(SDL_Event event) {
    SDL_MouseMotionEvent& motion = event.motion;

    if (motion.y <= camera->screenHeight / 2) {
        cameraUpdated = true;
        camera->pitchUp(motion.y);
    } else {
        cameraUpdated = true;
        camera->pitchDown(motion.y);
    }

    if (motion.x <= camera->screenWidth / 2) {
        cameraUpdated = true;
        camera->yawLeft(motion.x);
    } else {
        cameraUpdated = true;
        camera->yawRight(motion.x);
    }
}

void InputHandler::handleKeyboardEvent(SDL_Event event) {
    switch (event.key.keysym.sym) {
        case SDLK_ESCAPE:
            running = false;
            break;
        case SDLK_w:
            cameraUpdated = true;
            camera->moveForward();
            break;
        case SDLK_s:
            cameraUpdated = true;
            camera->moveBackward();
            break;
        case SDLK_a:
            cameraUpdated = true;
            camera->moveLeft();
            break;
        case SDLK_d:
            cameraUpdated = true;
            camera->moveRight();
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
