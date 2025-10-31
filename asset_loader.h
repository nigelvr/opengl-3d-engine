#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <utility>
#include <glm/glm.hpp>

struct Vertex {
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 uv;
};

struct MeshData {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
};

MeshData loadOBJ(const std::string& path);
size_t countUniqueEdges(const MeshData& mesh);

