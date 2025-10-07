#include "input_handler.h"
#include <SDL2/SDL.h>

InputHandler::InputHandler(Camera & cCam)
    : cam(cCam), running(true), cameraUpdated(false)
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

    if (motion.y <= cam.screenHeight / 2) {
        cameraUpdated = true;
        cam.pitchUp(motion.y);
    } else {
        cameraUpdated = true;
        cam.pitchDown(motion.y);
    }

    if (motion.x <= cam.screenWidth / 2) {
        cameraUpdated = true;
        cam.yawLeft(motion.x);
    } else {
        cameraUpdated = true;
        cam.yawRight(motion.x);
    }
}

void InputHandler::handleKeyboardEvent(SDL_Event event) {
    switch (event.key.keysym.sym) {
        case SDLK_ESCAPE:
            running = false;
            break;
        case SDLK_w:
            cameraUpdated = true;
            cam.moveForward();
            break;
        case SDLK_s:
            cameraUpdated = true;
            cam.moveBackward();
            break;
        case SDLK_a:
            cameraUpdated = true;
            cam.moveLeft();
            break;
        case SDLK_d:
            cameraUpdated = true;
            cam.moveRight();
            break;
        case SDLK_LEFT:
            cameraUpdated = true;
            cam.yaw -= 1;
            break;
        case SDLK_RIGHT:
            cameraUpdated = true;
            cam.yaw += 1;
            break;
        case SDLK_DOWN:
            cameraUpdated = true;
            cam.cameraPos.y -= 1;
            break;
        case SDLK_UP:
            cameraUpdated = true;
            cam.cameraPos.y -= 1;
            break;
        
    }
}
