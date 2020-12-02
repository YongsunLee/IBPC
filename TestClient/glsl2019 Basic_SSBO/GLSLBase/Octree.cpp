#include "stdafx.h"
#include "GameObject.h"
#include "Octree.h"

#define MAX_DEPTH 5

Octree::Octree()
{
	region = Box(glm::vec3(-1, 0, -1) * 0.5f, glm::vec3(1, 1, 1) * 0.5f);
}

Octree::Octree(Box region, std::vector<Vertex> particles, int id, int depth)
{
	this->region = region;

	int i = 0;
	for (auto p : particles) {
		this->mParticles.push_back(p);
		this->mParticleIDs.push_back(i);
		i++;
	}
	this->id = id;
	this->depth = depth;
	buildTree();
}

Octree::~Octree()
{
}

void Octree::buildTree()
{
	// MAX 깊이면
	if (depth == MAX_DEPTH) {
		isLeaf = 1;
		treeBuilt = true;
		return;
	}

	glm::vec3 dimensions = region.max - region.min;
	glm::vec3 half = dimensions * 0.5f;
	glm::vec3 center = glm::vec3(region.min) + half;

	// 자식노드 8개
	Box* octant = new Box[8];
	octant[0] = Box(region.min, center);
	octant[1] = Box(glm::vec3(center.x, region.min.y, region.min.z), glm::vec3(region.max.x, center.y, center.z));
	octant[2] = Box(glm::vec3(center.x, region.min.y, center.z), glm::vec3(region.max.x, center.y, region.max.z));
	octant[3] = Box(glm::vec3(region.min.x, region.min.y, center.z), glm::vec3(center.x, center.y, region.max.z));
	octant[4] = Box(glm::vec3(region.min.x, center.y, region.min.z), glm::vec3(center.x, region.max.y, center.z));
	octant[5] = Box(glm::vec3(center.x, center.y, region.min.z), glm::vec3(region.max.x, region.max.y, center.z));
	octant[6] = Box(center, region.max);
	octant[7] = Box(glm::vec3(region.min.x, center.y, center.z), glm::vec3(center.x, region.max.y, region.max.z));

	std::vector<std::vector<Vertex>> octLists;
	octLists.resize(8);
	
	// 파티클이 어느 자식노드에 속하는지
	for (int i = 0; i < 8; i++) {
		for (auto p : mParticles) {
			if (aabbVSParticle(octant[i], p)) {
				octLists[i].push_back(p);
			}
		}
	}

	for (int i = 0; i < 8; i++) {
		if (octLists[i].size()) {
			int temp = octLists[i].size() * i;
			children[i] = new Octree(octant[i], octLists[i], id, depth + 1);
			mParticleIDs.push_back(i);
		}
	}

	treeBuilt = true;
}

void Octree::getNodes(Octree* curr, std::vector<Node>& nodes, std::list<Octree*> toProcess)
{
	Node temp;
	temp.info.x = curr->mParticleIDs.size();
	temp.info.y = curr->isLeaf;
	temp.info.z = curr->depth;
	temp.region = curr->region;
	
	nodes.push_back(temp);
	if (!toProcess.empty()) {
		toProcess.pop_front();
	}

	getNodes(toProcess.front(), nodes, toProcess);

	for (int i = 0; i < curr->mParticleIDs.size(); ++i) {
		temp.particleRefs[i] = curr->mParticleIDs[i];
	}
}
