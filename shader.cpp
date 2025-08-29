#include "shader.h"

Shader::Shader(const char* vertexShaderFilename, const char* fragmentShaderFilename) {
    vertexShaderSource = (char*)read_file_data(vertexShaderFilename);
    fragmentShaderSource = (char*)read_file_data(fragmentShaderFilename);
}

Shader::~Shader() {
    glDeleteProgram(shaderId);
}

void Shader::compile() {
    // === Compile Vertex Shader ===
    vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShaderId, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShaderId);

    // === Compile Fragment Shader ===
    fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderId, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShaderId);

    // === Link Shaders into a Program ===
    shaderId = glCreateProgram();
    glAttachShader(shaderId, vertexShaderId);
    glAttachShader(shaderId, fragmentShaderId);
    glLinkProgram(shaderId);

    // Cleanup shader objects after linking
    glDeleteShader(vertexShaderId);
    glDeleteShader(fragmentShaderId);
}

void Shader::use() {
    glUseProgram(shaderId);
}

unsigned int Shader::getLocationPtr(std::string name) {
    return glGetUniformLocation(shaderId, name.c_str());
}

void Shader::installM4(std::string name, glm::mat4 mat) {
    glUniformMatrix4fv(getLocationPtr(name), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::installVec3(std::string name, glm::vec3 v) {
    glUniform3f(getLocationPtr(name), v.x, v.y, v.z);
}

void Shader::installVec3A(std::string name, glm::vec3* a, int size) {
    if (size > 0) {
        glUniform3fv(getLocationPtr(name), size, glm::value_ptr(a[0]));
    }
}

void Shader::installInt(std::string name, int i) {
    glUniform1i(getLocationPtr(name), i);
}

void Shader::installBool(std::string name, bool b) {
    installInt(name, b);
}
