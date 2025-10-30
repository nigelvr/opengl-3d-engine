#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <fstream>
#include <iostream>
#include "common.h"

Camera::Camera() {}

Camera::Camera(glm::vec3 cPos, glm::vec3 cFront, glm::vec3 cUp,
               float cFov, float cAr, double cYaw, double cPitch,
               int cScrWidth, int cScrHeight)
    : cameraPos(cPos), cameraFront(cFront), cameraUp(cUp),
      fov(cFov), ar(cAr), yaw(cYaw), pitch(cPitch),
      camSpeed(2.0f), screenWidth(cScrWidth), screenHeight(cScrHeight)
{
    updateFront();
}

Camera::Camera(std::string jsonPath) {
    json config;

    try {
        config = jsonData(jsonPath)["camera"];
    } catch (const json::parse_error& e) {
        std::cerr << e.what() << "\n";
        throw e;
    }

    cameraPos = parseVec3(config, "camera_pos");
    cameraFront = parseVec3(config, "camera_front");
    cameraUp = parseVec3(config, "camera_up");
    screenWidth = config["screen_w"].get<int>();
    screenHeight = config["screen_h"].get<int>();
    ar = (float)screenWidth / (float)screenHeight;
    yaw = config["yaw"].get<double>();
    pitch = config["pitch"].get<double>();
    fov = config["fov"].get<float>();
}


glm::mat4 Camera::viewMatrix() {
    return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

glm::mat4 Camera::projectionMatrix() {
    return glm::perspective(glm::radians(fov), ar, 0.1f, 100.0f);
}

void Camera::setCamSpeed(float s) {
    camSpeed = s;
}

void Camera::moveForward() {
    cameraPos += static_cast<float>(camSpeed) * cameraFront;
}

void Camera::moveBackward() {
    cameraPos -= static_cast<float>(camSpeed) * cameraFront;
}

void Camera::moveLeft()
{
    glm::vec3 right = glm::normalize(glm::cross(cameraFront, cameraUp));
    cameraPos -= right * camSpeed;
}
void Camera::moveRight()
{
    glm::vec3 right = glm::normalize(glm::cross(cameraFront, cameraUp));
    cameraPos += right * camSpeed;
}

void Camera::processMouseMovement(float xoffset, float yoffset, bool constrainPitch)
{
    constexpr float sensitivity = 0.08f;
    yaw   += xoffset * sensitivity;
    pitch += yoffset * sensitivity;

    if (constrainPitch) {
        if (pitch > 89.0) pitch = 89.0;
        if (pitch < -89.0) pitch = -89.0;
    }

    updateFront();
}

void Camera::updateFront() {
    glm::vec3 front;
    double yawRad = glm::radians(yaw);
    double pitchRad = glm::radians(pitch);

    front.x = static_cast<float>(cos(yawRad) * cos(pitchRad));
    front.y = static_cast<float>(sin(pitchRad));
    front.z = static_cast<float>(sin(yawRad) * cos(pitchRad));

    cameraFront = glm::normalize(front);
}