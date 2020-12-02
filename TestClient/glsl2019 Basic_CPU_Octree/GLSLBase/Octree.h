#pragma once

class OctreeNode {
public:
	OctreeNode();
	~OctreeNode();

	// Add Obejct
	void AddObject(Vertex* vertx);

	// Pos Check
	bool IsInNode(const glm::vec3 pos);
	bool IsInNode(glm::vec3 nodePos, float nodeWidth, const glm::vec3 pos);

	void update(float f);

	// Render
	void Render();

	// Set
		// Set Prev Node
	void SetParent(OctreeNode* const node) { m_pParent = node; }
	
		// Set Pos
    void SetPosition(glm::vec3 pos) { m_vPos = pos; }
    
		// Set Bounding Box width
	void SetWidth(float w) { m_fWidth = w; }

		// Set Next Node
    void AddChildNode(OctreeNode* node);

	void SetIsLeaf(bool b) { isLeaf = b; }

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


private:
	// Prev 1°³
	OctreeNode* m_pParent;
	
	// Next 8°³
	std::vector<OctreeNode*> m_vChildren;
	
	// Data
	std::vector<Vertex*> m_objects;

	bool isLeaf = true;
    glm::vec3 m_vPos;
    float m_fWidth;
};
