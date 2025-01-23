#define STB_IMAGE_IMPLEMENTATION

#include "texture.h"

#include "../libs/stb_image.h"

#include <stdexcept>

Texture::Texture(const std::string &filename) : filename(filename) { // NOLINT(*-pro-type-member-init)
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

	int width, height, channels;
	stbi_set_flip_vertically_on_load(1);
	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &channels, 0);
	if (!data) {
		stbi_image_free(data);
		throw std::runtime_error("Failed to load texture " + filename);
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	stbi_image_free(data);
}

Texture::~Texture() = default;