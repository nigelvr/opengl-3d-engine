#include <cmath>
#include <fstream>
#include <iostream>
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

json jsonData(std::string jsonPath) {
    std::ifstream ifs(jsonPath);
    return json::parse(ifs);
}

glm::vec3 parseVec3(json config, std::string key) {
    const auto& arr = config[key];
    return glm::vec3(arr[0].get<float>(), arr[1].get<float>(), arr[2].get<float>());
}

int main(int argc, char **argv) {
    json config = jsonData("./config.json");
    const auto& arr = config["foo"];
    for (size_t i = 0; i < arr.size(); ++i) {
        // Access each element as a double
        double value = arr[i].get<double>();
        std::cout << "Element " << i << ": " << value << "\n";
    }

    return 0;
}