
all: opengl_lighting

opengl_lighting: opengl_lighting.cpp vertex_shader.glsl fragment_shader.glsl
	g++ opengl_lighting.cpp -g -o opengl_lighting -lSDL2 -lGL -lGLEW -lglfw -lstb -I/usr/include/stb

.PHONY: clean
clean:
	rm -f opengl_lighting