#ifndef INCLUDE_FRAMEBUFFER_H
#define INCLUDE_FRAMEBUFFER_H

#include "ForwardDecs.h"

class Framebuffer
{
public:
	GLuint id;
	bool accessibleDepth;
	GLuint *colorTex;
	int numColorTex;
	GLuint depthTex;

	int width, height;

	Framebuffer(int w, int h, int numColorTexture, bool accessibleDepth);
	~Framebuffer();

	void deleteTextures();

	void resize(int w, int h);
	void bind(int, GLuint*);
	void unbind();

	void bindTexture(int slot, int index);
	void bindDepthTexture(int slot);

	void blitFramebuffer(int index, int x, int y, int dest_width, int dest_height);

private:
	void addColorTexture(int index);
	void addDepthBuffer();
	void addDepthTexture();

};

#endif