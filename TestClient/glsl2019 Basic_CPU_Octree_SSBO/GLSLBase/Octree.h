#pragma once

class OctreeNode {
public:
	struct Node {
		glm::vec4 aabb;
		glm::ivec4 info;
		glm::ivec4 vertexRef[2048]; 
	};

	OctreeNode();
	~OctreeNode();

	// Add Obejct
	void AddObject(Vertex* vertx);
	void AddObject(Vertex* vertx, int idx);

	// Pos Check
	bool IsInNode(const glm::vec3 pos);
	bool IsInNode(glm::vec3 nodePos, float nodeWidth, const glm::vec3 pos);

	// Re Build
	void Rebuild();

	// Set
		// Set Prev Node
	void SetParent(OctreeNode* const node) { m_pParent = node; }

	// Set Pos
	void SetPosition(glm::vec3 pos) { m_vPos = pos; }

	// Set Bounding Box width
	void SetWidth(float w) { m_fWidth = w; }

	// Set Next Node
	void AddChildNode(OctreeNode* node);

	// Set Leaf
	void SetIsLeaf(bool b) { isLeaf = b; }

	// Set Depth
	void SetDepth(int d) { m_depth = d; }

	// Get
		// Get Pos
	glm::vec3 const GetPos() { return m_vPos; }

	// Get Child Vector
	std::vector<OctreeNode*> const GetChild() { return m_vChildren; }

	// Get Child Index
	OctreeNode* const GetChildNode(int index);

	// Get Width
	float const GetWidth() { return m_fWidth; }

	// Get Parent
	OctreeNode* const GetParent() { return m_pParent; }

	// Get Object IDs
	std::vector<int> const GetObejctIDs() { return m_ObjectIDs; }
	std::vector<Vertex*> const GetObjectArray() { return m_objects; }

	//Get IsLeaf
	bool const GetIsLeaf() { return isLeaf; }

	int const GetDepth() { return  m_depth; }

private:
	// Prev 1°³
	OctreeNode* m_pParent;

	// Next 8°³
	std::vector<OctreeNode*> m_vChildren;

	// Data
	std::vector<Vertex*> m_objects;
	std::vector<int> m_ObjectIDs;

	bool isLeaf = false;

	glm::vec3 m_vPos;
	float m_fWidth;

	int m_depth = 0;
};
