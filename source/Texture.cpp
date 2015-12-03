#include "Texture.h"
#include <SOIL/SOIL.h>

Texture::Texture(std::string filename, bool srgb, GLenum wrap) {
    glGenTextures(1, &textureHandle);

    glBindTexture(GL_TEXTURE_2D, textureHandle);
    int width, height;
    unsigned char * image = SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_RGBA);
    unsigned char * buffer = static_cast<unsigned char *>(malloc(width * height * 4));
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            for (int i = 0; i < 4; i++)
            {
                buffer[(x + (height - y - 1) * width) * 4 + i] = image[(x + y * width) * 4 + i];
            }
        }
    }
    glTexImage2D(GL_TEXTURE_2D, 0, srgb ? GL_SRGB_ALPHA : GL_RGBA, width, height, 0, GL_RGBA,
        GL_UNSIGNED_BYTE, buffer);
    free(buffer);
    SOIL_free_image_data(image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glGenerateMipmap(GL_TEXTURE_2D);

    GLfloat anisoAmt = 0.0f;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &anisoAmt);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisoAmt);

	glBindTexture(GL_TEXTURE_2D, 0);
    CHECK_ERROR();
}

Texture::Texture(GLuint handle) : textureHandle(handle) {
}

Texture::Texture(glm::vec4 color)
{
    glGenTextures(1, &textureHandle);

    char buffer[4] = { color.r * 255, color.g * 255, color.b * 255, color.a * 255 };

    glBindTexture(GL_TEXTURE_2D, textureHandle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA,
        GL_UNSIGNED_BYTE, buffer);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glGenerateMipmap(GL_TEXTURE_2D);

    GLfloat anisoAmt = 0.0f;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &anisoAmt);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisoAmt);

    glBindTexture(GL_TEXTURE_2D, 0);
    CHECK_ERROR();
}

Texture::~Texture()
{
    glDeleteTextures(1, &textureHandle);
}

void Texture::bindTexture(int slot) const {
	int textureSlot = GL_TEXTURE0 + slot;
	glActiveTexture(textureSlot);
	glBindTexture(GL_TEXTURE_2D, textureHandle);
    CHECK_ERROR();
}