#pragma once
#include <GL/glew.h>
#include <cstdio>

// stb_image forward declaration
// Ensure you include stb_image.h in your build
// #define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

class Texture {
public:
    unsigned int texture;

    Texture(const char* textureFilename);

private:
    void initOpenglTextureObjects(const char* textureFilename);
};