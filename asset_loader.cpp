#include "asset_loader.h"



MeshData loadOBJ(const std::string& path) {
    std::ifstream file(path);
    if (!file) throw std::runtime_error("Cannot open OBJ file: " + path);

    std::vector<glm::vec3> temp_positions;
    std::vector<glm::vec2> temp_uvs;
    std::vector<glm::vec3> temp_normals;
    std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string type;
        ss >> type;

        if (type == "v") {
            glm::vec3 v; ss >> v.x >> v.y >> v.z;
            temp_positions.push_back(v);
        } else if (type == "vt") {
            glm::vec2 uv; ss >> uv.x >> uv.y;
            temp_uvs.push_back(uv);
        } else if (type == "vn") {
            glm::vec3 n; ss >> n.x >> n.y >> n.z;
            temp_normals.push_back(n);
        } else if (type == "f") {
            std::string vtn;
            for (int i = 0; i < 3; ++i) {
                ss >> vtn;
                unsigned int vIdx, tIdx, nIdx;
                sscanf(vtn.c_str(), "%u/%u/%u", &vIdx, &tIdx, &nIdx);
                vertexIndices.push_back(vIdx);
                uvIndices.push_back(tIdx);
                normalIndices.push_back(nIdx);
            }
        }
    }

    MeshData mesh;
    for (size_t i = 0; i < vertexIndices.size(); ++i) {
        Vertex v;
        v.pos = temp_positions[vertexIndices[i] - 1];
        v.uv  = temp_uvs[uvIndices[i] - 1];
        v.normal = temp_normals[normalIndices[i] - 1];
        mesh.vertices.push_back(v);
        mesh.indices.push_back(static_cast<unsigned int>(i));
    }

    return mesh;
}

size_t countUniqueEdges(const MeshData& mesh) {
    std::set<std::pair<unsigned int, unsigned int>> edges;

    for (size_t i = 0; i < mesh.indices.size(); i += 3) {
        unsigned int a = mesh.indices[i];
        unsigned int b = mesh.indices[i + 1];
        unsigned int c = mesh.indices[i + 2];

        auto addEdge = [&](unsigned int v1, unsigned int v2) {
            // store with smaller index first for uniqueness
            if (v1 > v2) std::swap(v1, v2);
            edges.insert({v1, v2});
        };

        addEdge(a, b);
        addEdge(b, c);
        addEdge(c, a);
    }

    return edges.size();
}

/* std::vector<int> meshEdges(MeshData &mesh) {
    std::vector<int> edges;

}
*/