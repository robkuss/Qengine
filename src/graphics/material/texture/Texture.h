#pragma once

using namespace std;

#include <string>

#define GLFW_INCLUDE_GLEXT
#include <GL/glew.h>


class Texture {
public:
	explicit Texture(const string &filename);

	~Texture() = default;

	GLuint id{};

private:
	string filename;
};
