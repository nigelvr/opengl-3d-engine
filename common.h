#include <stdio.h>
#include <utility>
#include <string>
#include <fstream>
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

json jsonData(std::string jsonPath);
glm::vec3 parseVec3(json config, std::string key);

const char *read_file_data(std::string filename);