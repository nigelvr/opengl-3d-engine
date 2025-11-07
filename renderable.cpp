#include <iostream>
#include <unordered_set>
#include <set>

#include "renderable.h"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"

std::vector<unsigned int> generateEdgeIndices(const std::vector<unsigned int>& triangleIndices) {
    std::vector<unsigned int> edgeIndices;
    std::set<std::pair<unsigned int, unsigned int>> uniqueEdges;

    for (size_t i = 0; i < triangleIndices.size(); i += 3) {
        unsigned int i0 = triangleIndices[i];
        unsigned int i1 = triangleIndices[i + 1];
        unsigned int i2 = triangleIndices[i + 2];

        // Ensure consistent ordering: smaller index first
        auto addEdge = [&uniqueEdges, &edgeIndices](unsigned int a, unsigned int b) {
            if (a > b) std::swap(a, b);
            if (uniqueEdges.insert({a, b}).second) {
                edgeIndices.push_back(a);
                edgeIndices.push_back(b);
            }
        };

        addEdge(i0, i1);
        addEdge(i1, i2);
        addEdge(i2, i0);
    }

    return edgeIndices;
}

std::tuple<std::vector<Vertex>, std::vector<unsigned int>, std::vector<unsigned int>> parseObjFile(std::string filename) {
    std::vector<glm::vec3> vx_coords;
    std::vector<unsigned int> vx_idxs;
    std::vector<glm::vec3> normal_coords;
    std::vector<int> normal_idxs;
    std::vector<glm::vec2> uv_coords;
    std::vector<int> uv_idxs;

    std::ifstream file(filename);
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream linestream(line);
        std::string type;
        linestream >> type;
        glm::vec3 v;
        glm::vec2 u;
        if (type == "v") {
            linestream >> v.x >> v.y >> v.z;
            vx_coords.push_back(v);
        } else if (type == "vt") {
            linestream >> u.x >> u.y;
            uv_coords.push_back(u);
        } else if (type == "vn") {
            linestream >> v.x >> v.y >> v.z;
            normal_coords.push_back(v);
        } else if (type == "f") {
            std::string part;
            for (int i = 0; i < 3; i++) {
                linestream >> part;
                int v,t,n;
                sscanf(part.c_str(), "%u/%u/%u", &v, &t, &n);
                vx_idxs.push_back(v);
                uv_idxs.push_back(t);
                normal_idxs.push_back(n);
            }
        }
    }

    std::vector<Vertex> vxs;
    std::vector<unsigned int> idxs;
    glm::vec3 v;
    glm::vec2 u;
    glm::vec3 n;
    std::map<std::tuple<unsigned int, unsigned int, unsigned int>, unsigned int> vertexMap;
    for (int i = 0; i < vx_idxs.size(); i++) {
        auto key = std::make_tuple(vx_idxs[i], normal_idxs[i], uv_idxs[i]);
        if (vertexMap.count(key) == 0) {
            vertexMap[key] = vxs.size();
            v = vx_coords[vx_idxs[i]-1];
            n = normal_coords[normal_idxs[i]-1];
            u = uv_coords[uv_idxs[i]-1];
            vxs.push_back(Vertex(v,n,u));
        }
        idxs.push_back(vertexMap[key]);
    }

    std::vector<unsigned int> edgeIndices = generateEdgeIndices(vx_idxs);

    return std::make_tuple(vxs, idxs, edgeIndices);
}

void Renderable::draw(std::shared_ptr<Shader> shader, bool wireFrame)
{
    shader->installM4("model", model);
    shader->installVec3("objectColor", color);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, numVertexIndices, GL_UNSIGNED_INT, 0);

    if (wireFrame) {
        shader->installVec3("objectColor", glm::vec3(0.0f, 0.0f, 0.0f));
        glLineWidth(10.0);
        glBindVertexArray(VAO_edges);
        glDrawElements(GL_LINES, numEdgeIndices, GL_UNSIGNED_INT, 0);
    }

    // reset state
    glBindVertexArray(0);
}

Mesh::Mesh(std::string objfile, glm::vec3 position, glm::vec3 color) {
    this->color = color;
    this->model = glm::translate(glm::mat4(1.0f), position);

    auto objdata = parseObjFile(objfile);
    std::vector<Vertex> vxs = std::get<0>(objdata);
    std::vector<unsigned int> idxs = std::get<1>(objdata);
    std::vector<unsigned int> edgeIdxs = std::get<2>(objdata);
    numVertexIndices = idxs.size();
    numEdgeIndices = edgeIdxs.size();

    printf("numEdgeIndices = %d\n", numEdgeIndices);

    // Set up memory for the object
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vxs.size()*sizeof(Vertex), vxs.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, idxs.size()*sizeof(unsigned int), idxs.data(), GL_STATIC_DRAW);

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    // normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    // uv
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
    glEnableVertexAttribArray(2);

    // Set up memory for edges
    glGenVertexArrays(1, &VAO_edges);
    glGenBuffers(1, &EBO_edges);

    glBindVertexArray(VAO_edges);

    // Bind same vertex buffer again (shared data)
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Bind & fill element buffer for edges
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_edges);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numEdgeIndices*sizeof(unsigned int), edgeIdxs.data(), GL_STATIC_DRAW);

    // Vertex attribute layout (identical to faces)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // --- 6. Unbind for safety ---
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &VAO_edges);
    glDeleteBuffers(1, &EBO_edges);
}