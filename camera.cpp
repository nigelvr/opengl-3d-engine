#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

json jsonData(std::string jsonPath) {
    std::ifstream ifs(jsonPath);
    return json::parse(ifs);
}

Camera::Camera() {}

Camera::Camera(glm::vec3 cPos, glm::vec3 cFront, glm::vec3 cUp,
               float cFov, float cAr, double cYaw, double cPitch,
               int cScrWidth, int cScrHeight)
    : cameraPos(cPos), cameraFront(cFront), cameraUp(cUp),
      fov(cFov), ar(cAr), yaw(cYaw), pitch(cPitch),
      camSpeed(2.0f), screenWidth(cScrWidth), screenHeight(cScrHeight)
{}

Camera::Camera(std::string jsonPath) {
    json config;

    try {
        config = jsonData(jsonPath);
    } catch (const json::parse_error& e) {
        std::cerr << e.what() << "\n";
        throw e;
    }

    cameraPos = glm::vec3(config["camera_x"].get<float>(), config["camera_y"].get<float>(), config["camera_z"].get<float>());
    cameraFront = glm::vec3(config["camera_front_x"].get<float>(), config["camera_front_y"].get<float>(), config["camera_front_z"].get<float>());
    cameraUp = glm::vec3(config["camera_up_x"].get<float>(), config["camera_up_y"].get<float>(), config["camera_up_z"].get<float>());
    screenWidth = config["screen_w"].get<int>();
    screenHeight = config["screen_h"].get<int>();
    ar = (float)screenWidth / (float)screenHeight;
    yaw = config["yaw"].get<double>();
    pitch = config["pitch"].get<double>();
    fov = config["fov"].get<float>();
}

glm::vec3 Camera::direction() {
    return cameraPos + cameraFront;
}

glm::mat4 Camera::viewMatrix() {
    /* float yawRad = glm::radians(static_cast<float>(yaw));
    float pitchRad = glm::radians(static_cast<float>(pitch));

    cameraFront.x = cos(yawRad) * cos(pitchRad);
    cameraFront.y = sin(pitchRad);
    cameraFront.z = sin(yawRad) * cos(pitchRad); */
    cameraFront = -cameraPos;

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

void Camera::moveLeft() {
    cameraPos += glm::normalize(glm::cross(cameraUp, cameraFront)) * static_cast<float>(camSpeed) * 2.5f;
}

void Camera::moveRight() {
    cameraPos -= glm::normalize(glm::cross(cameraUp, cameraFront)) * static_cast<float>(camSpeed) * 2.5f;
}

void Camera::pitchUp(int motion_y) {
    pitch = ((static_cast<float>(screenHeight) / 2.0f - motion_y) / (static_cast<float>(screenHeight) / 2.0f)) * 180.0f;
}

void Camera::pitchDown(int motion_y) {
    pitch = -(motion_y - static_cast<float>(screenHeight) / 2.0f) / (static_cast<float>(screenHeight) / 2.0f) * 180.0f;
}

void Camera::yawLeft(int motion_x) {
    yaw = -90.0f + -180.0f * ((static_cast<float>(screenWidth) / 2.0f - motion_x) / (static_cast<float>(screenWidth) / 2.0f));
}

void Camera::yawRight(int motion_x) {
    yaw = -90.0f - -180.0f * ((motion_x - static_cast<float>(screenWidth) / 2.0f) / (static_cast<float>(screenWidth) / 2.0f));
}
