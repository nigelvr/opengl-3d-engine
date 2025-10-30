#pragma once
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    glm::vec3 cameraPos;
    glm::vec3 cameraFront;
    glm::vec3 cameraUp;
    float fov;
    float ar;
    double yaw;
    double pitch;
    float camSpeed;
    int screenWidth;
    int screenHeight;

    Camera();
    Camera(glm::vec3 cPos, glm::vec3 cFront, glm::vec3 cUp,
           float cFov, float cAr, double cYaw, double cPitch,
           int cScrWidth, int cScrHeight);
    Camera(std::string jsonPath);

    glm::mat4 viewMatrix();
    glm::mat4 projectionMatrix();

    void setCamSpeed(float s);
    void moveForward();
    void moveBackward();
    void moveLeft();
    void moveRight();
    void processMouseMovement(float xoffset, float yoffset, bool constrainPitch);
private:
    void updateFront();
    
};
