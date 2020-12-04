#pragma once
#include "Vertex.h"

class Mesh {
public:
	Mesh();
	virtual ~Mesh();

	bool load(const std::string& filepath);
	void bind();
	void render();

	glm::mat4 const GetAABBTransform() { return mAABBTransform; };
	glm::vec3 const GetAABBCenterPos() { return mAABBCenter; };

protected:
	std::vector<Vertex>			mVertices;
	std::vector<unsigned int>	mIndices;
	GLuint						mVBO;
	GLuint						mIBO;

	glm::vec2					mAxisX;
	glm::vec2					mAxisY;
	glm::vec2					mAxisZ;

	glm::mat4					mAABBTransform;
	glm::vec3					mAABBCenter;
};