#include "Texture.h";

Texture::Texture(std::string filename) {
	glGenTextures(1, &textureHandle);
	glBindTexture(GL_TEXTURE_2D, textureHandle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::loadTextureFile(std::string filename) {
	//TODO
}

void Texture::bindTexture(int slot) {
	int textureSlot = GL_TEXTURE0 + slot;
	glActiveTexture(textureSlot);
	glBindTexture(GL_TEXTURE_2D, textureHandle);
}