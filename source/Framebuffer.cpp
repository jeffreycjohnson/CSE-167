#include "Framebuffer.h"
#include "Renderer.h"
#include <gtc/matrix_transform.hpp>

Framebuffer::Framebuffer(int w, int h, int numColorTextures, bool accessibleDepth) : width(w), height(h), numColorTex(numColorTextures), accessibleDepth(accessibleDepth) {
	glGenFramebuffers(1, &id);
	glBindFramebuffer(GL_FRAMEBUFFER, id);

	colorTex = new GLuint[numColorTex];

	for (int x = 0; x < numColorTex; ++x) {
		addColorTexture(x);
	}

	if (accessibleDepth) {
		addDepthTexture();
	} else {
		addDepthBuffer();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Framebuffer::addColorTexture(int index) {
	glGenTextures(1, &(colorTex[index]) );
	glBindTexture(GL_TEXTURE_2D, colorTex[index]);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, colorTex[index], 0);
}

void Framebuffer::addDepthTexture() {
	glGenTextures(1, &depthTex);
	glBindTexture(GL_TEXTURE_2D, depthTex);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	/* For Hardware PCF
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL); */

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTex, 0);
}

void Framebuffer::addDepthBuffer() {
	glGenRenderbuffers(1, &depthTex);
	glBindRenderbuffer(GL_RENDERBUFFER, depthTex);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthTex);
}

Framebuffer::~Framebuffer() {
	deleteTextures();
	glDeleteFramebuffers(1, &id);
	delete[] colorTex;
}

void Framebuffer::deleteTextures()
{
	glDeleteTextures(numColorTex, colorTex);
	if (accessibleDepth) {
		glDeleteTextures(1, &depthTex);
	} else {
		glDeleteRenderbuffers(1, &depthTex);
	}
}

void Framebuffer::bind(int bufferCount, GLuint *buffersToDraw) {

	glBindFramebuffer(GL_FRAMEBUFFER, id);
	glViewport(0, 0, width, height);

	glDrawBuffers(bufferCount, buffersToDraw);

	//TODO - necessary?
	glm::mat4 perspective = glm::perspective((float)(atan(1)*4.0f / 3.0f), width / (float)height, .1f, 100.f);
	(Renderer::getCurrentShader())["uP_Matrix"] = perspective;
}

void Framebuffer::unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, Renderer::getWindowWidth(), Renderer::getWindowHeight());

	//TODO remove if not changed above
	glm::mat4 perspective = glm::perspective((float)(atan(1)*4.0f / 3.0f), Renderer::getWindowWidth() / (float)Renderer::getWindowHeight(), .1f, 100.f);
	(Renderer::getCurrentShader())["uP_Matrix"] = perspective;
}

void Framebuffer::bindTexture(int slot, int colorIndex) {
	glActiveTexture(GL_TEXTURE0 + slot);
	GLuint tex = colorTex[colorIndex];
	glBindTexture(GL_TEXTURE_2D, tex);
}

void Framebuffer::bindDepthTexture(int slot) {
	glActiveTexture(GL_TEXTURE0 + slot);
	GLuint tex = depthTex;
	glBindTexture(GL_TEXTURE_2D, tex);
}

void Framebuffer::blitFramebuffer(int index, int x, int y, int destW, int destH) {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, id);
	glReadBuffer(GL_COLOR_ATTACHMENT0 + index);
	glBlitFramebuffer(0, 0, width, height, x, y, x+destW, y+destH, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
}

void Framebuffer::resize(int w, int h) {
	width = w;
	height = h;

	deleteTextures();

	glBindFramebuffer(GL_FRAMEBUFFER, id);
	for (int x = 0; x < numColorTex; ++x) {
		addColorTexture(x);
	}

	if (accessibleDepth) {
		addDepthTexture();
	} else {
		addDepthBuffer();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}