#pragma once

class Octree {
private:
	struct Node {
		Box region;
		glm::ivec3 info;
		int particleRefs[2048]; // 
	};

	Octree();
	Octree(Box region, std::vector<Vertex> particles, int id, int depth);
	virtual ~Octree();
	void buildTree();
	//void renderOctree(ShaderProgram* shader, Octree* current, Mesh* box);
	void getNodes(Octree* curr, std::vector<Node>& nodes, std::list<Octree*> toProcess);

	Box region;

	std::vector<Vertex> mParticles;
	std::vector<int> mParticleIDs;
	
	Octree* children[8];
	
	int depth = 0;
	int isLeaf = 0;

	const float minSize = 0.01;
	bool treeBuilt = false;
	int id = -1;

	inline double _fRand(double fMin, double fMax)
	{
		double f = (double)rand() / RAND_MAX;
		return fMin + f * (fMax - fMin);
	}

	inline bool aabbVSParticle(Box& aabb, Vertex& particle) {
		bool inside = false;
		for (int i = 0; i < 3; i++) {
			if (aabb.min.x <= particle.pos.x && aabb.max.x >= particle.pos.x &&
				aabb.min.y <= particle.pos.y && aabb.max.y >= particle.pos.y &&
				aabb.min.z <= particle.pos.z && aabb.max.z >= particle.pos.z) {
				inside = true;
			}
		}

		return inside;
	}
};