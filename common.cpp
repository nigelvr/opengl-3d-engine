#include "common.h"

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