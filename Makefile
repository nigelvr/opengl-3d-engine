# Compiler and flags
CXX      := g++
CXXFLAGS := -g -I/usr/include/stb
LDFLAGS  := -lSDL2 -lGL -lGLEW -lglfw -lstb

# Sources and objects
SRC      := camera.cpp input_handler.cpp opengl_lighting.cpp
OBJ      := $(SRC:.cpp=.o)
TARGET   := opengl_lighting

# Shader files (not compiled, but included in dependencies)
SHADERS  := vertex_shader.glsl fragment_shader.glsl

# Default target
all: $(TARGET)

# Link final binary
$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $@ $(LDFLAGS)

# Generic rule for compiling .cpp -> .o
%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build artifacts
.PHONY: clean
clean:
	rm -f $(OBJ) $(TARGET)
