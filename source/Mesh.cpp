#include "Mesh.h"
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include "GameObject.h"
#include "Shader.h"

#include "Renderer.h"

#define FLOAT_SIZE 4
#define MV_MATRIX "uMV_Matrix"

std::unordered_map<std::string, MeshData> Mesh::meshMap;

Mesh::Mesh(std::string filename) : filename(filename) {
	loadObjFile(filename);
}

Mesh::~Mesh() {

}


void Mesh::draw(glm::mat4 cameraMat) {
	MeshData& currentEntry = meshMap.at(filename);
	Shader& currentShader = Renderer::getCurrentShader();

	glBindVertexArray(currentEntry.vaoHandle);

	currentShader[MV_MATRIX] = cameraMat * gameObject->transform.getTransformMatrix();


	glDrawElements(GL_TRIANGLES, currentEntry.indexSize, GL_UNSIGNED_INT, 0);
}





//Split functions from the interwebs
//http://stackoverflow.com/questions/236129/split-a-string-in-c
std::vector<std::string>& split(const std::string &s, char delim, std::vector<std::string> &elems)
{
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim))
	{
		elems.push_back(item);
	}
	return elems;
}

std::vector<std::string> split(const std::string &s, char delim)
{
	std::vector<std::string> elems;
	split(s, delim, elems);
	return elems;
}

void Mesh::loadObjFile(std::string filename) {
	if (meshMap.find(filename) != meshMap.end()) {
		return;
	}

	std::vector<float> megaArray;
	std::vector<int> indexArray;

	std::ifstream infile(filename);
	std::string line;
	std::vector<std::string> tokens;
	std::string token;

	std::vector<glm::vec3>tmpVertices;
	std::vector<glm::vec3>tmpNormals;
	std::unordered_map<std::string, int> faceMap;
	int currentIndex = 0;
	int optimization = 0;

	int lineNum = 0;

	float minX, minY, minZ, maxX, maxY, maxZ;
	minX = minY = minZ = std::numeric_limits<float>::max();
	maxX = maxY = maxZ = std::numeric_limits<float>::max() * -1;


	std::cout << "Starting parse..." << std::endl;

	//While all your lines are belong to us
	while (std::getline(infile, line))
	{

		//Progress
		if (++lineNum % 10000 == 0)
			std::cout << "At line " << lineNum << std::endl;

		//Split a line into tokens by delimiting it on spaces
		//"Er Mah Gerd" becomes ["Er", "Mah", "Gerd"]
		tokens.clear();
		tokens = split(line, ' ', tokens);

		if (tokens.size() == 0) continue;

		//If first token is a v then it gots to be a vertex
		if (tokens.at(0).compare("v") == 0)
		{
			//Parse the vertex line
			float x = std::stof(tokens.at(1));
			float y = std::stof(tokens.at(2));
			float z = std::stof(tokens.at(3));

			if (x < minX) minX = x;
			if (y < minY) minY = y;
			if (z < minZ) minZ = z;

			if (x > maxX) maxX = x;
			if (y > maxY) maxY = y;
			if (z > maxZ) maxZ = z;

			tmpVertices.push_back(glm::vec3(x, y, z));

		}
		else if (tokens.at(0).compare("vn") == 0)
		{
			//Parse the normal line
			float a = std::stof(tokens.at(1));
			float b = std::stof(tokens.at(2));
			float c = std::stof(tokens.at(3));
			tmpNormals.push_back(glm::normalize(glm::vec3(a, b, c)));
		}
		else if (tokens.at(0).compare("f") == 0)
		{
			for (int i = 0; i < 3; ++i) {
				std::string currentString = tokens.at(i + 1);
				auto entry = faceMap.find(currentString);
				if (entry != faceMap.end()) {
					optimization++;
					indexArray.push_back((*entry).second);
					continue;
				}

				std::vector<std::string> index_tokens;
				index_tokens = split(tokens.at(i + 1), '/', index_tokens);

				int vertIndex = std::stoi(index_tokens[0]) - 1;
				int normalIndex = std::stoi(index_tokens[2]) - 1;

				glm::vec3 vertData = tmpVertices.at(vertIndex);
				for (int dataIndex = 0; dataIndex < 3; ++dataIndex) {
					megaArray.push_back(vertData[dataIndex]);
				}

				glm::vec3 normalData = tmpNormals.at(normalIndex);
				for (int dataIndex = 0; dataIndex < 3; ++dataIndex) {
					megaArray.push_back(normalData[dataIndex]);
				}

				faceMap[currentString] = currentIndex;
				indexArray.push_back(currentIndex++);
			}

		}

	}


	glm::vec3 center(minX + (maxX - minX) / 2.0, minY + (maxY - minY) / 2.0, minZ + (maxZ - minZ) / 2.0);
	glm::vec3 size((maxX - minX) / 2.0, (maxY - minY) / 2.0, (maxZ - minZ) / 2.0);



	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(glGetAttribLocation(Renderer::getCurrentShader().id, "aPosition"));
	glEnableVertexAttribArray(glGetAttribLocation(Renderer::getCurrentShader().id, "aNormal"));

	GLuint meshBuffer[2];
	glGenBuffers(2, meshBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, meshBuffer[0]);
	glBufferData(GL_ARRAY_BUFFER, megaArray.size() * sizeof(float), &(megaArray[0]), GL_STATIC_DRAW);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshBuffer[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexArray.size() * sizeof(int), &(indexArray[0]), GL_STATIC_DRAW);


	int stride = FLOAT_SIZE * 6;
	glVertexAttribPointer(0, 3, GL_FLOAT, false, stride, (GLvoid*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, false, stride, (GLvoid*)(FLOAT_SIZE * 3));

	
	//meshData.vertDataHandle = meshBuffer[0];
	//meshData.indexHandle = meshBuffer[1];

	MeshData meshData;
	meshData.vaoHandle = vao;
	meshData.indexSize = indexArray.size();
	meshMap[filename] = meshData;

}

