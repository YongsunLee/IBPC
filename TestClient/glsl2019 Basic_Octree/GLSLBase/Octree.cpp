#include "stdafx.h"
#include "GameObject.h"
#include "Octree.h"

OctreeNode::OctreeNode()
{
	m_pParent = nullptr;
    m_fWidth = 0.0f;
}

OctreeNode::~OctreeNode()
{

}

void OctreeNode::AddChildNode(OctreeNode* node)
{
    m_vChildren.push_back(node);
}

void OctreeNode::AddObject(Vertex* vertx)
{
    m_objects.push_back(vertx);
}

OctreeNode* const OctreeNode::GetChildNode(int index)
{
    return m_vChildren[index];
}

bool OctreeNode::IsInNode(const glm::vec3 pos)
{
	float fMin, fMax;

	// x축 최대, 최소, y축 최대, 최소, z축 최대 최소
	for (int i = 0; i < 3; ++i) {
		// Node의 포지션에 반지름을 이용하여 확인
		fMin = m_vPos[i] - m_fWidth;
		fMax = m_vPos[i] + m_fWidth;

		if (pos[i] < fMin || pos[i] > fMax) {
			return FALSE;
		}
	}

	return TRUE;
}

bool OctreeNode::IsInNode(const OctreeNode* node, const glm::vec3 pos)
{
	bool inside = false;
	for (int i = 0; i < 3; i++) {
		if (node->m_BoundingBox.min.x <= pos.x && node->m_BoundingBox.max.x >= pos.x &&
			node->m_BoundingBox.min.y <= pos.y && node->m_BoundingBox.max.y >= pos.y &&
			node->m_BoundingBox.min.z <= pos.z && node->m_BoundingBox.max.z >= pos.z) {
			inside = true;
		}
	}

	return inside;
}

void OctreeNode::Render()
{
	
}
