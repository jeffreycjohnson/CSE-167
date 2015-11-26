#include "Texture.h"
#include <SOIL/SOIL.h>

Texture::Texture(std::string filename) {
    textureHandle = SOIL_load_OGL_texture(filename.c_str(),
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_TEXTURE_REPEATS);
	glBindTexture(GL_TEXTURE_2D, textureHandle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    GLfloat anisoAmt = 0.0f;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &anisoAmt);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisoAmt);

	glBindTexture(GL_TEXTURE_2D, 0);
    CHECK_ERROR();
}

void Texture::loadTextureFile(std::string filename) {
	//TODO
}

void Texture::bindTexture(int slot) const {
	int textureSlot = GL_TEXTURE0 + slot;
	glActiveTexture(textureSlot);
	glBindTexture(GL_TEXTURE_2D, textureHandle);
    CHECK_ERROR();
}