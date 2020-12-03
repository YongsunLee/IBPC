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

void OctreeNode::AddObject(Vertex* vertex)
{
	// ���࿡ ���ϵ尡 ������ (isLeaf)
	// �� ���ؽ��� child�� ���ԵǴ��� Ȯ���� addobject

	int childIndex = -1;

	if (!isLeaf)
	{
		for (int i = 0 ; i < 8; ++i) 
		{
			if (IsInNode(m_vChildren[i]->GetPos(), m_vChildren[i]->GetWidth(), vertex->pos))
			{
				childIndex = i;
				break;
			};
		}
		
		if (childIndex != -1) m_vChildren[childIndex]->AddObject(vertex);
	}
	else 
	{
		if (IsInNode(vertex->pos))
			m_objects.push_back(vertex);
	}
}

OctreeNode* const OctreeNode::GetChildNode(int index)
{
    return m_vChildren[index];
}

bool OctreeNode::IsInNode(const glm::vec3 pos)
{
	float fMin, fMax;

	// x�� �ִ�, �ּ�, y�� �ִ�, �ּ�, z�� �ִ� �ּ�
	for (int i = 0; i < 3; ++i) {
		// Node�� �����ǿ� �������� �̿��Ͽ� Ȯ��
		fMin = m_vPos[i] - m_fWidth;
		fMax = m_vPos[i] + m_fWidth;

		if (pos[i] < fMin || pos[i] > fMax) {
			return FALSE;
		}
	}

	return TRUE;
}

bool OctreeNode::IsInNode(glm::vec3 nodePos, float nodeWidth, const glm::vec3 pos)
{
	glm::vec3 aabbMin;
	glm::vec3 aabbMax;

	aabbMin = nodePos - glm::vec3(nodeWidth);
	aabbMax = nodePos + glm::vec3(nodeWidth);

	bool inside = false;
	if (aabbMin.x <= pos.x && aabbMax.x >= pos.x &&
		aabbMin.y <= pos.y && aabbMax.y >= pos.y &&
		aabbMin.z <= pos.z && aabbMax.z >= pos.z) {
		inside = true;
	}

	return inside;
}

void OctreeNode::update(float f)
{
	if (!isLeaf)
	{
		for (int i = 0 ; i < 8; ++i)
		{
			m_vChildren[i]->update(f);
		}
	}
	else
	{
		if (m_objects.size() != 0)
		{
			int i = 0;
			for (auto& p : m_objects)
			{
				p->pos = p->pos + ((p->dir * p->speed) + (f * p->speed * glm::vec3(0, -1, 0)));

				if (IsInNode(p->pos))
					m_objects.erase(m_objects.begin() + i);

				i += 1;
			}
		}
	}
	
}

void OctreeNode::Render()
{
	
}