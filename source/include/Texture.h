#ifndef INCLUDE_TEXTURE_H
#define INCLUDE_TEXTURE_H

#include "ForwardDecs.h"

class Texture
{
	public:
		unsigned int textureHandle;

        explicit Texture(std::string filename);
		~Texture();

		void bindTexture(int slot) const;


};

#endif