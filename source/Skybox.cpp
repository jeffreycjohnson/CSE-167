#include "Skybox.h"
#include "Renderer.h"
#include <SOIL.h>
#include "stb_image.h" //note - can get this file from SOIL2 - just put in an include folder in Dependencies for now
#include "Renderer.h"
#include "Camera.h"

#define FLOAT_SIZE 4
#define POSITION_COUNT 3

#define VERTEX_ATTRIB_LOCATION 0

#define VERTEX_COUNT 12
float vertices[VERTEX_COUNT] = { -1, -1, 0,
1, -1, 0,
1,  1, 0,
-1,  1, 0 };

#define INDEX_COUNT 6
GLuint indices[INDEX_COUNT] = { 0, 1, 2, 0, 2, 3 };

bool Skybox::loaded = false;
MeshData Skybox::meshData;



void Skybox::load() {
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glEnableVertexAttribArray(VERTEX_ATTRIB_LOCATION);

	GLuint meshBuffer[2];
	glGenBuffers(2, meshBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, meshBuffer[0]);
	glBufferData(GL_ARRAY_BUFFER, VERTEX_COUNT * sizeof(float), vertices, GL_STATIC_DRAW);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshBuffer[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, INDEX_COUNT * sizeof(GLuint), indices, GL_STATIC_DRAW);


	int stride = FLOAT_SIZE * (POSITION_COUNT);
	glVertexAttribPointer(VERTEX_ATTRIB_LOCATION, 3, GL_FLOAT, false, stride, (GLvoid*)0);

	meshData.vaoHandle = vao;
	meshData.indexSize = static_cast<GLsizei>(INDEX_COUNT);
}

void Skybox::draw() {
	if (!loaded) {
		load();
	}


	Renderer::switchShader(SKYBOX_SHADER);
	(*Renderer::getShader(SKYBOX_SHADER))["uV_Matrix"] = Renderer::camera->getCameraMatrix();

	if (Renderer::gpuData.vaoHandle != meshData.vaoHandle) {
		glBindVertexArray(meshData.vaoHandle);
		Renderer::gpuData.vaoHandle = meshData.vaoHandle;
	}

	glDrawElements(GL_TRIANGLES, meshData.indexSize, GL_UNSIGNED_INT, 0);
}

GLuint Skybox::loadCubemap(glm::mat4(&irradianceMatrix)[3], std::string imageFiles[6]) {
	GLuint retVal;
	retVal = SOIL_load_OGL_cubemap
		(
			imageFiles[0].c_str(),
			imageFiles[1].c_str(),
			imageFiles[2].c_str(),
			imageFiles[3].c_str(),
			imageFiles[4].c_str(),
			imageFiles[5].c_str(),
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB
			);

	loadIrradiance(irradianceMatrix, imageFiles);
	return retVal;
}

#define CUBE_FACES 6
#define SH_COUNT 9
void Skybox::loadIrradiance(glm::mat4(&irradianceMatrix)[3], std::string imageFiles[6]) {
	//for each cube map
	glm::vec3 irradiance[9];
	float* currentImage;
	int currentWidth;
	int currentHeight;
	int channels;
	static float shConst[9] = { 0.282095, 0.488603, 0.488603, 0.488603, 1.092548, 1.092548, 1.092548, 0.315392, 0.546274 };

	float xVal, yVal, zVal;

	for (int m = 0; m < CUBE_FACES; ++m) {
		//load image
		//currentImage = SOIL_load_image(imageFiles[m].c_str(), &currentWidth, &currentHeight, &channels, SOIL_LOAD_AUTO);
		currentImage = stbi_loadf(imageFiles[m].c_str(), &currentWidth, &currentHeight, &channels, 0);

		for (int y = 0; y < currentHeight; ++y) {
			float yPercent = y / (float)currentHeight;

			for (int x = 0; x < currentWidth; ++x) {
				float xPercent = x / (float)currentWidth;

				switch (m) {
				case 0: //rt
					xVal = 1;
					yVal = 2 * yPercent - 1;
					zVal = -(2 * xPercent - 1);
					break;
				case 1: //lf
					xVal = -1;
					yVal = 2 * yPercent - 1;
					zVal = 2 * xPercent - 1;
					break;
				case 2: //up
					xVal = 2 * xPercent - 1;
					yVal = 1;
					zVal = -(2 * yPercent - 1);
					break;
				case 3: //dn
					xVal = 2 * xPercent - 1;
					yVal = -1;
					zVal = 2 * yPercent - 1;
					break;
				case 4: //bk
					xVal = -(2 * xPercent - 1);
					yVal = 2 * yPercent - 1;
					zVal = -1;
					break;
				case 5: //ft
					xVal = (2 * xPercent - 1);
					yVal = 2 * yPercent - 1;
					zVal = 1;
					break;
				}

				float mag = sqrt(xVal*xVal + yVal*yVal + zVal * zVal);
				xVal /= mag;
				yVal /= mag;
				zVal /= mag;

				float theta = acos(zVal / sqrt(xVal*xVal + yVal*yVal + zVal*zVal));

				float currentSH;
				for (int shIndex = 0; shIndex < SH_COUNT; ++shIndex) {
					switch (shIndex) {
					case 0: //0,0
						currentSH = shConst[shIndex];
						break;
					case 1: //1,-1
						currentSH = shConst[shIndex] * yVal;
						break;
					case 2: //1,0
						currentSH = shConst[shIndex] * zVal;
						break;
					case 3: //1,1
						currentSH = shConst[shIndex] * xVal;
						break;
					case 4: //2, -2
						currentSH = shConst[shIndex] * xVal * yVal;
						break;
					case 5: //2, -1
						currentSH = shConst[shIndex] * yVal * zVal;
						break;
					case 6: //2, 0
						currentSH = shConst[shIndex] * (3 * zVal*zVal - 1);
						break;
					case 7: //2, 1
						currentSH = shConst[shIndex] * xVal * zVal;
						break;
					case 8: //2, 2
						currentSH = shConst[shIndex] * (xVal*xVal - yVal*yVal);
						break;
					}
					for (int c = 0; c < 3; ++c) {
						irradiance[shIndex][c] += (currentSH * sin(theta) / (CUBE_FACES*currentWidth*currentHeight)) * (currentImage[(x + y*currentWidth)*channels + c]);
					}
				}
			}
		}
		free(currentImage);
	}

	float c1 = 0.429043;
	float c2 = 0.511664;
	float c3 = 0.743125;
	float c4 = 0.886227;
	float c5 = 0.247708;

	for (int c = 0; c < 3; ++c) {
		irradianceMatrix[c] = glm::mat4(c1*irradiance[8][c], c1*irradiance[4][c], c1*irradiance[7][c], c2*irradiance[3][c],
			c1*irradiance[4][c], -c1*irradiance[8][c], c1*irradiance[5][c], c2*irradiance[1][c],
			c1*irradiance[7][c], c1*irradiance[5][c], c3*irradiance[6][c], c2*irradiance[2][c],
			c2*irradiance[3][c], c2*irradiance[1][c], c2*irradiance[2][c], c4*irradiance[0][c] - c5*irradiance[6][c]);
	}
}
