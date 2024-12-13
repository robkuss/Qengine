#pragma once

#define GLFW_INCLUDE_GLEXT

#include <GL/glew.h>

#include <string>

class Texture {
public:
	explicit Texture(const std::string &filename);
	~Texture();

	GLuint id{};

private:
	std::string filename;
};
