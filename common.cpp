#include "common.h"

const char *read_file_data(std::string filename) {
    FILE *fd = fopen(filename.c_str(), "r");
    
    fseek(fd, 0, SEEK_END);
    long length = ftell(fd);
    rewind(fd);

    char *text = (char *)calloc(length, sizeof(char));
    size_t bytes_read = fread((char *)text, sizeof(char), length, fd);

    fclose(fd);

    return (const char *)text;
}

json jsonData(std::string jsonPath) {
    std::ifstream ifs(jsonPath);
    return json::parse(ifs);
}

glm::vec3 parseVec3(json config, std::string key) {
    const auto& arr = config[key];
    return glm::vec3(arr[0].get<float>(), arr[1].get<float>(), arr[2].get<float>());
}