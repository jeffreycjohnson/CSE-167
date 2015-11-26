#ifndef INCLUDE_TEXTURE_H
#define INCLUDE_TEXTURE_H

#include "ForwardDecs.h"

class Texture
{
	public:
		unsigned int textureHandle;

		Texture(std::string filename);
		~Texture();

		void loadTextureFile(std::string filename);

		void bindTexture(int slot) const;


};

#endif