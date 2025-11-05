#include <iostream>

#include "renderable.h"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"

std::pair<std::vector<float>, std::vector<int>> extractVerticies(std::string filename) {
    std::ifstream file(filename);
    
    std::vector<float> vx_coords;
    std::vector<int> vx_idxs;
    std::string line;

    while (std::getline(file, line)) {
        std::istringstream linestream(line);
        std::string type;
        linestream >> type;

        if (type == "v") {
            glm::vec3 v;
            linestream >> v.x >> v.y >> v.z;
            vx_coords.push_back(v.x);
            vx_coords.push_back(v.y);
            vx_coords.push_back(v.z);
        } else if (type == "f") {
            std::string part;
            for (int i = 0; i < 3; i++) {
                linestream >> part;
                int v,t,n;
                sscanf(part.c_str(), "%u/%u/%u", &v, &t, &n);
                vx_idxs.push_back(v-1);
            }
        }
    }
    return std::make_pair(vx_coords, vx_idxs);
}

void Renderable::draw(std::shared_ptr<Shader> shader)
{
    shader->installM4("model", model);
    shader->installVec3("objectColor", color);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}


SimpleCube::SimpleCube(glm::vec3 position, glm::vec3 color)
{
    this->color = color;
    this->model = glm::translate(glm::mat4(1.0f), position);

    setupMesh();
}

SimpleCube::~SimpleCube()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void SimpleCube::setupMesh()
{
    unsigned int indices[] = {
        0, 1, 2, 2, 3, 0,   // back
        4, 5, 6, 6, 7, 4,   // front
        0, 1, 5, 5, 4, 0,   // bottom
        2, 3, 7, 7, 6, 2,   // top
        0, 3, 7, 7, 4, 0,   // left
        1, 2, 6, 6, 5, 1    // right
    };

    auto meshdata = extractVerticies("assets/models/cube.obj");
    auto vxs = meshdata.first;
    auto idxs = meshdata.second;

    for (int i = 0; i < vxs.size(); i++) {
        if (i % 3 == 0) {
            printf("\n");
        }
        printf("%f ", vxs[i]);
    }
    printf("\n");

    for (int i = 0; i < idxs.size(); i++) {
        if (i % 6 == 0) {
            printf("\n");
        }
        printf("%d ", idxs[i]);
    }
    printf("\n");



    numIndices = sizeof(indices) / sizeof(indices[0]);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vxs.size()*sizeof(float), vxs.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, idxs.size()*sizeof(int), idxs.data(), GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

WireCube::WireCube(glm::vec3 position,
                   glm::vec3 color,
                   glm::vec3 edgeColor,
                   float edgeThickness)
    : color(color),
      edgeColor(edgeColor),
      edgeThickness(edgeThickness)
{
    model = glm::translate(glm::mat4(1.0f), position);
    setupMesh();
}

WireCube::~WireCube() {
    glDeleteVertexArrays(1, &VAO_faces);
    glDeleteVertexArrays(1, &VAO_edges);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO_faces);
    glDeleteBuffers(1, &EBO_edges);
}

void WireCube::draw(std::shared_ptr<Shader> shader)
{
    shader->use();
    shader->installM4("model", model);
    shader->installVec3("objectColor", color);
    glBindVertexArray(VAO_faces);
    glDrawElements(GL_TRIANGLES, numFaceIndices, GL_UNSIGNED_INT, 0);

    // --- Draw bold edges ---
    shader->installVec3("objectColor", edgeColor);
    glLineWidth(edgeThickness);
    glBindVertexArray(VAO_edges);
    glDrawElements(GL_LINES, numEdgeIndices, GL_UNSIGNED_INT, 0);

    // reset state
    glBindVertexArray(0);
}


// ------------------------------------------------------------
// WireCube::setupMesh()
// ------------------------------------------------------------
// Sets up VAOs, VBOs, and EBOs for drawing both the cube’s
// filled faces and its wireframe edges.
// ------------------------------------------------------------
void WireCube::setupMesh() {
    // --- 1. Define cube vertex positions ---
    // Each vertex is a corner of the cube centered at origin.
    // Using normalized coordinates (-0.5 to +0.5) for easy scaling later.
    float vertices[] = {
        // positions           // normals           // UVs
        // Back face (-Z)
        -0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   0.0f, 0.0f,  // bottom-left
        0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   1.0f, 0.0f,  // bottom-right
        0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   1.0f, 1.0f,  // top-right
        -0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   0.0f, 1.0f,  // top-left

        // Front face (+Z)
        -0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   0.0f, 1.0f,

        // Left face (-X)
        -0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,

        // Right face (+X)
        0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,   1.0f,  0.0f,  0.0f,   0.0f, 1.0f,

        // Bottom face (-Y)
        -0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,   0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,   1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,   1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,   0.0f, 1.0f,

        // Top face (+Y)
        -0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,   0.0f, 0.0f,
        0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,   1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,   0.0f, 1.0f
    };

    // --- 2. Define triangle faces (for solid cube) ---
    // Each face has 2 triangles, made from 6 indices.
    unsigned int faceIndices[] = {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4,
        12, 13, 14, 14, 15, 12,
        16, 17, 18, 18, 19, 16,
        8, 9, 10, 10, 11, 8,
        20, 21, 22, 22, 23, 20,
    };

    // --- 3. Define line edges (for wireframe overlay) ---
    unsigned int edgeIndices[] = {
        0, 1, 1, 2, 2, 3, 3, 0,  // back
        4, 5, 5, 6, 6, 7, 7, 4,  // front
        0, 4, 1, 5, 2, 6, 3, 7   // connectors
    };

    // --- 4. Count elements for drawing later ---
    numFaceIndices = sizeof(faceIndices) / sizeof(faceIndices[0]);
    numEdgeIndices = sizeof(edgeIndices) / sizeof(edgeIndices[0]);

    // --- 5. Create and fill a single shared VBO (positions) ---
    // Both VAOs (faces and edges) will reference the same vertex buffer.
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // ------------------------------------------------------------
    // --- VAO #1 : Faces (solid triangles)
    // ------------------------------------------------------------
    glGenVertexArrays(1, &VAO_faces);
    glGenBuffers(1, &EBO_faces);

    glBindVertexArray(VAO_faces);

    // Bind shared vertex buffer (already filled)
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Bind & fill element buffer for faces
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_faces);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(faceIndices), faceIndices, GL_STATIC_DRAW);

    // Vertex attribute layout (location = 0,1)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);  // position

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);  // normal


    // ------------------------------------------------------------
    // --- VAO #2 : Edges (lines)
    // ------------------------------------------------------------
    glGenVertexArrays(1, &VAO_edges);
    glGenBuffers(1, &EBO_edges);

    glBindVertexArray(VAO_edges);

    // Bind same vertex buffer again (shared data)
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Bind & fill element buffer for edges
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_edges);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(edgeIndices), edgeIndices, GL_STATIC_DRAW);

    // Vertex attribute layout (identical to faces)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // --- 6. Unbind for safety ---
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

