#pragma once

#define GLFW_INCLUDE_GLEXT
#include <GL/glew.h>

using namespace std;
#include <string>


class Texture {
public:
	explicit Texture(const string &filename);

	~Texture() = default;

	GLuint id{};

private:
	string filename;
};
