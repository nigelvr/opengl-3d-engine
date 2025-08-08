#include <stdio.h>
#include <utility>
#include <string>

#include <SDL2/SDL.h>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stb_image.h>

#define SCR_WIDTH 800
#define SCR_HEIGHT 600

const char *read_file_data(const char *filename) {
    FILE *fd = fopen(filename, "r");
    
    fseek(fd, 0, SEEK_END);
    long length = ftell(fd);
    rewind(fd);

    char *text = (char *)calloc(length, sizeof(char));
    size_t bytes_read = fread((char *)text, sizeof(char), length, fd);

    fclose(fd);

    return (const char *)text;
}

class Shader {
public:
    char *vertexShaderSource;
    char *fragmentShaderSource;
    unsigned int vertexShaderId;
    unsigned int fragmentShaderId;
    unsigned int shaderId;

    Shader(const char *vertexShaderFilename, const char *fragmentShaderFilename) {
        vertexShaderSource = (char *)read_file_data(vertexShaderFilename);
        fragmentShaderSource = (char *)read_file_data(fragmentShaderFilename);
    }

    ~Shader() {
        glDeleteProgram(shaderId);
    }

    void compile() {
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

    void use() {
        glUseProgram(shaderId);
    }

    unsigned int getLocationPtr(std::string name) {
        return glGetUniformLocation(shaderId, name.c_str());
    }

    void installM4(std::string name, glm::mat4 mat) {
        glUniformMatrix4fv(getLocationPtr(name), 1, GL_FALSE, glm::value_ptr(mat));
    }

    void installVec3(std::string name, glm::vec3 v) {
        glUniform3f(getLocationPtr(name), v.x, v.y, v.z);
    }

    void installVec3A(std::string name, glm::vec3 *a, int size) {
        if (size > 0) {
            glUniform3fv(getLocationPtr(name), size, glm::value_ptr(a[0]));
        }
    }

    void installInt(std::string name, int i) {
        glUniform1i(getLocationPtr(name), i);
    }

    void installBool(std::string name, bool b) {
        installInt(name, b);
    }
};

class VertexData {
public:
    float *vertices;
    int num_vertices;
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;

    VertexData(const char *vertexDataFilename) {
        vertexRawData = read_file_data(vertexDataFilename);

        num_vertices = 0;
        count_vertices();
        
        vertices = (float *)calloc(num_vertices, sizeof(float));
        read_vertices();

        initOpenglVertexObjects();

    }

    ~VertexData() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        free(vertices);
    }
private:
    const char *vertexRawData;

    void count_vertices() {
        for (int i = 0; i < strlen(vertexRawData); i++) {
            if (vertexRawData[i] == ',') {
                num_vertices++;
            }
        }
        num_vertices++;
    }

    void read_vertices() {
        char buf[8];
        memset(buf, 0, 8);
        int j=0, k = 0;
        for (int i = 0; i <= strlen(vertexRawData); i++) {
            if (vertexRawData[i] == ',' || i == strlen(vertexRawData)) {
                vertices[k++] = atof(buf);
                memset(buf, 0, 8);
                j=0;
            } else {
                buf[j++] = vertexRawData[i];
            }
        }
    }

    void initOpenglVertexObjects() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, num_vertices*sizeof(float), vertices, GL_STATIC_DRAW);

        // position attribute (location = 0)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // position attribute (location = 1)
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
    }
};

class Texture {
public:
    unsigned int texture;

    Texture(const char *textureFilename) {
        initOpenglTextureObjects(textureFilename);
    }
private:
    void initOpenglTextureObjects(const char *textureFilename) {
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // load image, create texture and generate mipmaps
        int width, height, nrChannels;
        // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
        unsigned char *data = stbi_load(textureFilename, &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            printf("Failed to load texture\n");
        }
        stbi_image_free(data);
    }
};

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

    Camera() {}

    Camera(glm::vec3 cPos, glm::vec3 cFront, glm::vec3 cUp, float cFov, float cAr, double cYaw, double cPitch) {
        cameraPos = cPos;
        cameraFront = cFront;
        cameraUp = cUp;
        fov = cFov;
        ar = cAr;
        yaw = cYaw;
        pitch = cPitch;
    }

    glm::vec3 direction() {
        return cameraPos + cameraFront;
    }

    glm::mat4 viewMatrix() {
        float yawRad = glm::radians(static_cast<float>(yaw));
        float pitchRad = glm::radians(static_cast<float>(pitch));

        cameraFront.x = cos(yawRad) * cos(pitchRad);
        cameraFront.y = sin(pitchRad);
        cameraFront.z = sin(yawRad) * cos(pitchRad);

        return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    }

    glm::mat4 projectionMatrix() {
        return glm::perspective(glm::radians(fov), ar, 0.1f, 100.0f);
    }

    void setCamSpeed(float s) {
        camSpeed = s;
    }

    void moveForward() {
        cameraPos += (float)camSpeed * cameraFront;
    }

    void moveBackward() {
        cameraPos -= (float)camSpeed * cameraFront;
    }

    void moveLeft() {
        cameraPos += glm::normalize(glm::cross(cameraUp, cameraFront)) * (float)camSpeed * 2.5f;
    }

    void moveRight() {
        cameraPos -= glm::normalize(glm::cross(cameraUp, cameraFront)) * (float)camSpeed * 2.5f;
    }

    void pitchUp(SDL_MouseMotionEvent motion) {
        pitch = ((float)SCR_HEIGHT/2.0 - motion.y)/(float)SCR_HEIGHT/2.0 * 180.0f;
    }

    void pitchDown(SDL_MouseMotionEvent motion) {
        pitch = -(motion.y - (float)SCR_HEIGHT/2.0)/(float)SCR_HEIGHT/2.0 * 180.0f;
    }

    void yawLeft(SDL_MouseMotionEvent motion) {
        yaw = -90.0f + -180.0f * ((float)SCR_WIDTH/2.0 - motion.x)/((float)SCR_WIDTH/2.0);
    }

    void yawRight(SDL_MouseMotionEvent motion) {
        yaw = -90.0f - -180.0f * (motion.x - (float)SCR_WIDTH/2.0)/((float)SCR_WIDTH/2.0);
    }
};

class InputHandler {
public:
    bool running;
    bool cameraUpdated;
    Camera & cam;

    InputHandler(Camera & cCam) : cam(cCam), running(true), cameraUpdated(false) {}

    void processInput() {
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

    void handleMouseEvent(SDL_Event event) {
        SDL_MouseMotionEvent& motion = event.motion;

        if (motion.y <= SCR_HEIGHT/2) {
            cameraUpdated = true;
            cam.pitchUp(motion);
        } else {
            cameraUpdated = true;
            cam.pitchDown(motion);
        }

        if (motion.x <= SCR_WIDTH/2) {
            cameraUpdated = true;
            cam.yawLeft(motion);
        } else {
            cameraUpdated = true;
            cam.yawRight(motion);
        }
    }

    void handleKeyboardEvent(SDL_Event event) {
        switch (event.key.keysym.sym) {
            case SDLK_ESCAPE:
                running = false;
                break;
            case SDLK_UP:
                cameraUpdated = true;
                cam.moveForward();
                break;
            case SDLK_DOWN:
                cameraUpdated = true;
                cam.moveBackward();
                break;
            case SDLK_LEFT:
                cameraUpdated = true;
                cam.moveLeft();
                break;
            case SDLK_RIGHT:
                cameraUpdated = true;
                cam.moveRight();
                break;
        }
    }
};

class Cube {
public:
    glm::vec3 pos;
    glm::vec3 color;
    float scale;
    VertexData & vertexData;
    Shader & shader;

    Cube(glm::vec3 cubePos, glm::vec3 cubeColor, float cubeScale, VertexData & vd, Shader & cubeShader) : 
        pos(cubePos), color(cubeColor), vertexData(vd), shader(cubeShader), scale(cubeScale) {}

    glm::mat4 model() {
        auto m = glm::mat4(1.0f);
        m = glm::translate(m, pos);
        m = glm::scale(m, glm::vec3(scale));
        return m;
    }

    void draw() {
        shader.installM4("model", model());
        shader.installVec3("objectColor", color);
        shader.installBool("isLamp", false);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
};

class Lamp : public Cube {
public:
    Lamp(glm::vec3 pos, VertexData & vd, Shader & shader) : Cube(pos, glm::vec3(1.0f,1.0f,1.0f), 0.2f, vd, shader) {}
    Lamp(glm::vec3 pos, glm::vec3 color, VertexData & vd, Shader & shader) : Cube(pos, color, 0.2f, vd, shader) {}

    void draw() {
        shader.installM4("model", model());
        shader.installVec3("lightPos", pos);
        shader.installVec3("lightColor", color);
        shader.installBool("isLamp", true);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
};

void clearScreen() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void updateLights(Lamp *lamps, int numLights, Shader & shader) {
    glm::vec3 *lightPositions = new glm::vec3[numLights];
    for (int i = 0; i < numLights; i++) {
        lightPositions[i] = lamps[i].pos;
    }
    shader.installVec3A("lightSources", lightPositions, numLights);
    delete lightPositions;
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("OpenGL Triangle",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCR_WIDTH, SCR_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);    

    SDL_GLContext context = SDL_GL_CreateContext(window);
    glewExperimental = GL_TRUE;
    glewInit();

    glEnable(GL_DEPTH_TEST);  

    // === Define Triangle Vertex Data ===
    VertexData vertexData("./cube.data");
    Shader shader("vertex_shader.glsl", "fragment_shader.glsl");
    shader.compile();
    shader.use();

    // Camera and matricies
    glm::vec3 cameraPos = glm::vec3(-5.086209f, 0.003457f, -2.374545f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    double yaw = 1.35f;
    double pitch = 0.0f;
    float fov = 45.0f;
    float ar = (float)SCR_WIDTH / (float)SCR_HEIGHT;
    Camera camera(cameraPos, cameraFront, cameraUp, fov, ar, yaw, pitch);
    shader.installVec3("cameraPos", camera.cameraPos);

    // compute view, proj and model matrices
    // initialize model to be the identy matrix. update in the loop
    glm::mat4 view = camera.viewMatrix();
    glm::mat4 projection = camera.projectionMatrix();
    glm::mat4 model = glm::mat4(1.0f);
    shader.installM4("view", view);
    shader.installM4("projection", projection);
    shader.installM4("model", model);

    // light and box color
    auto objectColor = glm::vec3(1.0f, 0.5f, 0.31f);
    auto lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 lightPos(0.0f, 0.0f, 1.0f);
    shader.installVec3("objectColor", objectColor);
    shader.installVec3("lightColor", lightColor);
    shader.installVec3("lightPos", lightPos);
    
    // our world
    Cube cubes[] = {
        Cube(glm::vec3( 0.0f,  0.0f, 0.0f), glm::vec3(1.0f, 0.5f, 0.31f), 1.0f, vertexData, shader),
    };
    int numCubes = sizeof(cubes)/sizeof(cubes[0]);
    Lamp lightSources[] = {
        Lamp(glm::vec3(0.0f, -1.0f, 0.0f), vertexData, shader),
        Lamp(glm::vec3(0.0f, 1.0f, 0.0f), vertexData, shader),
        Lamp(glm::vec3(0.0f, -1.0f, 0.0f), vertexData, shader)
    };
    int numLights = sizeof(lightSources)/sizeof(lightSources[0]);
    shader.installInt("numLights", numLights);
    updateLights(lightSources, numLights, shader);

    // input handler
    InputHandler ih(camera);

    // === Render Loop ===
    int curTick, lastTick=0, deltaTick;
    double deltaTime = 0.1f;

    while (ih.running) {
        printf("cam = %f %f %f ; yaw = %f\n", camera.cameraPos.x, camera.cameraPos.y, camera.cameraPos.z, camera.yaw);
        curTick = SDL_GetTicks();
        // set cam speed
        deltaTick = curTick-lastTick;
        lastTick = curTick;
        deltaTime = (float)deltaTick/1000.0f;
        camera.setCamSpeed(deltaTime);
        // get keyboard + mouse input
        ih.processInput();
        if (ih.cameraUpdated) {
            view = camera.viewMatrix();
            shader.installM4("view", view);
            shader.installVec3("cameraPos", camera.cameraPos);
        }
        // clear screen before drawing
        clearScreen();
        // update the world
        // move the lamp up and down
        int tick = SDL_GetTicks();
        float time = tick/1000.0f;
        lightSources[0].pos = glm::vec3(sin(2.0f*time), 0.0f, cos(2.0f*time));
        lightSources[1].pos = glm::vec3(0.0f, sin(2.0f*time), cos(2.0f*time));
        lightSources[2].pos = glm::vec3(sin(2.0f*time), cos(2.0f*time), 0.0f);
        // draw the world
        updateLights(lightSources, numLights, shader);
        for (auto & lightSource : lightSources) {
            lightSource.draw();
        }
        for (auto & cube : cubes) {
            cube.draw();
        }

        SDL_GL_SwapWindow(window);
        deltaTime = glfwGetTime() - deltaTime;
    }

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

