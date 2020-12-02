#pragma once

class OctreeNode {
public:
	OctreeNode();
	~OctreeNode();

	// Set Prev Node
	void SetParent(OctreeNode* const node) { m_pParent = node; }
	
	// Set Pos
    void SetPosition(glm::vec3 pos) { m_vPos = pos; }
    
	// Set Bounding Box width
	void SetWidth(float w) { m_fWidth = w; }

	// Set Next Node
    void AddChildNode(OctreeNode* node);

	// Add Obejct
	void AddObject(Vertex* vertx);

	// Pos Check
	bool IsInNode(const glm::vec3 pos);
	bool IsInNode(const OctreeNode* node, const glm::vec3 pos);

	// Render
	void Render();

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
	Box m_BoundingBox;

	// Prev 1°³
	OctreeNode* m_pParent;
	
	// Next 8°³
	std::vector<OctreeNode*> m_vChildren;
	
	// Data
	std::list<Vertex*> m_objects;

    glm::vec3 m_vPos;
    float m_fWidth;
};
