#include <stdio.h>
#include <SDL2/SDL.h>

#include <stb_image.h>

#include <utility>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
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
        auto dir = direction();
        
        float yawRad = glm::radians(static_cast<float>(yaw));
        float pitchRad = glm::radians(static_cast<float>(pitch));
        // glm::vec3 front;

        cameraFront.x = cos(yawRad) * cos(pitchRad);
        cameraFront.y = sin(pitchRad);
        cameraFront.z = sin(yawRad) * cos(pitchRad);

        return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    }

    glm::mat4 projectionMatrix() {
        return glm::perspective(glm::radians(fov), ar, 0.1f, 100.0f);
    }
};

void handleKeyboardEvent(SDL_Event event, Camera & cam, double camSpeed, bool *running, bool *cameraUpdated) {
    switch (event.key.keysym.sym) {
        case SDLK_ESCAPE:
            *running = false;
            break;
        case SDLK_UP:
            *cameraUpdated = true;
            cam.cameraPos += (float)camSpeed * cam.cameraFront;
            break;
        case SDLK_DOWN:
            *cameraUpdated = true;
            cam.cameraPos -= (float)camSpeed * cam.cameraFront;
            break;
        case SDLK_LEFT:
            *cameraUpdated = true;
            cam.cameraPos += glm::normalize(glm::cross(cam.cameraUp, cam.cameraFront)) * (float)camSpeed * 2.5f;
            break;
        case SDLK_RIGHT:
            *cameraUpdated = true;
            cam.cameraPos -= glm::normalize(glm::cross(cam.cameraUp, cam.cameraFront)) * (float)camSpeed * 2.5f;
            break;
    }
}

void processInput(Camera & cam, double camSpeed, bool *running, bool *cameraUpdated) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            *running = false;
        } else if (event.type == SDL_KEYDOWN) {
            handleKeyboardEvent(event, cam, camSpeed, running, cameraUpdated);
        } else if (event.type == SDL_MOUSEMOTION) {
            SDL_MouseMotionEvent& motion = event.motion;

            if (motion.y <= SCR_HEIGHT/2) {
                *cameraUpdated = true;
                cam.pitch = ((float)SCR_HEIGHT/2.0 - motion.y)/(float)SCR_HEIGHT/2.0 * 90.0f;
            } else {
                *cameraUpdated = true;
                cam.pitch = -(motion.y - (float)SCR_HEIGHT/2.0)/(float)SCR_HEIGHT/2.0 * 90.0f;
            }

            if (motion.x <= SCR_WIDTH/2) {
                *cameraUpdated = true;
                cam.yaw = -90.0f + -180.0f * ((float)SCR_WIDTH/2.0 - motion.x)/((float)SCR_WIDTH/2.0);
            } else {
                *cameraUpdated = true;
                cam.yaw = -90.0f - -180.0f * (motion.x - (float)SCR_WIDTH/2.0)/((float)SCR_WIDTH/2.0);
            }
        }
    }
}

class Cube {
public:
    float sideLength;
    glm::vec3 pos;
    glm::vec3 color;
    Cube(float cubeSideLength, glm::vec3 cubePos, glm::vec3 cubeColor) {
        sideLength = cubeSideLength;
        pos = cubePos;
        color = cubeColor;
    }
};

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
    glm::vec3 cameraPos = glm::vec3(-2.466283f, 0.000000f, 2.922494f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    double yaw = -64.35f;
    double pitch = 0.0f;
    float fov = 45.0f;
    float ar = (float)SCR_WIDTH / (float)SCR_HEIGHT;
    Camera camera(cameraPos, cameraFront, cameraUp, fov, ar, yaw, pitch);

    // compute view and proj matrices
    // model is computed in the render loop
    glm::mat4 view = camera.viewMatrix();
    glm::mat4 projection = camera.projectionMatrix();
    unsigned int viewLoc  = glGetUniformLocation(shader.shaderId, "view");
    unsigned int projectionLoc = glGetUniformLocation(shader.shaderId, "projection");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    // initialize the model matrix
    glm::mat4 model;

    // light and box color
    auto objectColor = glm::vec3(1.0f, 0.5f, 0.31f);
    auto lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    unsigned int objectColorLoc = glGetUniformLocation(shader.shaderId, "objectColor");
    unsigned int lightColorLoc = glGetUniformLocation(shader.shaderId, "lightColor");
    glUniform3f(objectColorLoc, objectColor.x, objectColor.y, objectColor.z);
    glUniform3f(lightColorLoc, lightColor.x, lightColor.y, lightColor.z);

    // cube positions
    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f, -2.0f)
    };
    int numBoxes = sizeof(cubePositions)/sizeof(cubePositions[0]);

    // === Render Loop ===
    bool running = true;
    bool cameraUpdated;
    SDL_Event event;
    int curTick, lastTick=0, deltaTick;
    double deltaTime = 0.1f;
    while (running) {
        curTick = SDL_GetTicks();
        deltaTick = curTick-lastTick;
        lastTick = curTick;

        deltaTime = (float)deltaTick/1000.0f;

        processInput(camera, deltaTime, &running, &cameraUpdated);
        if (cameraUpdated) {
            view = camera.viewMatrix();
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        }

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float time = SDL_GetTicks() / 1000.0f;  // seconds since start
        float angle = time;                     // rotate 1 radian per second

        glBindVertexArray(vertexData.VAO);
        unsigned int modelLoc;
        for (int k = 0; k < numBoxes; k++) {
            modelLoc = glGetUniformLocation(shader.shaderId, "model");
            model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[k]);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        SDL_GL_SwapWindow(window);
        deltaTime = glfwGetTime() - deltaTime;
    }

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

