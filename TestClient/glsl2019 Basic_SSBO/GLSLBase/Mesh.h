#pragma once
#include "Vertex.h"

class Mesh {
public:
	Mesh();
	virtual ~Mesh();

	bool load(const std::string& filepath);
	void bind();
	void render();

protected:
	std::vector<Vertex>			mVertices;
	std::vector<unsigned int>	mIndices;
	GLuint						mVBO;
	GLuint						mIBO;
};

struct Box {
	Box(glm::vec3 min, glm::vec3 max) {
		this->min = glm::vec4(min, 1);
		this->max = glm::vec4(max, 1);
	}
	Box() {
		this->min = glm::vec4(0, 0, 0, 1);
		this->max = glm::vec4(0, 0, 0, 1);
	}
	glm::vec4 min;
	glm::vec4 max;
};