#pragma once
#include "common.h"
#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader {
public:
    char* vertexShaderSource;
    char* fragmentShaderSource;
    unsigned int vertexShaderId;
    unsigned int fragmentShaderId;
    unsigned int shaderId;

    Shader();
    Shader(const char* vertexShaderFilename, const char* fragmentShaderFilename);
    ~Shader();

    void compile();
    void use();
    unsigned int getLocationPtr(std::string name);
    void installM4(std::string name, glm::mat4 mat);
    void installVec3(std::string name, glm::vec3 v);
    void installVec3A(std::string name, glm::vec3* a, int size);
    void installInt(std::string name, int i);
    void installBool(std::string name, bool b);
};

class LightingShader : public Shader {
public:
    LightingShader() : Shader("vertex_shader.glsl", "fragment_shader.glsl") {
        compile();
        use();
    }
};