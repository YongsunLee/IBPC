#pragma once

#include "Light.h"
#include "ShaderProgram.h"
#include "Mesh.h"
#include "Octree.h"
#include "ShaderStorageBufferObject.h"
#include "Texture.h"

class Camera;
class GameObject;
class OctreeNode;
class ShaderStorageBufferObject;
class Texture;

class Renderer
{
public:
	Renderer(int windowSizeX, int windowSizeY);
	~Renderer();

	// Camera Set
	void SetCamera(Camera* pCamera) { m_pCamera = pCamera; };
	
	void Test();
	void DrawCube();
	void DrawCube(glm::vec3 pos, float width);
	void DrawCube(GameObject& obj);

	// Draw Mesh Object
	void DrawObject(GameObject& obj);
	
	void UpdateSSBO();
	void DrawSSBOParticle();

	void UpdateOctree();
	void UpdateNodeTexture();
	void DrawOctreee();

	// Draw All Object
	void DrawSystem();
	OctreeNode* BuildOctree(glm::vec3 vCenter, FLOAT fHalfWidth, int depthLimit);
	
	bool m_UpdateSwitch = false;

private:
	void Initialize(int windowSizeX, int windowSizeY);
	void CreateVertexBufferObjects(); 

	void CreateSceneObject();
	void CreateParticleSSBO();

private:
	bool m_Initialized = false;

	unsigned int m_ClientWidth = 0;
	unsigned int m_ClientHeight = 0;

	// Camera
	Camera* m_pCamera;

	// Shader
		// Test Shader
	ShaderProgram m_SolidRectShader;
	ShaderProgram m_CubeShader;

		// Shader to Use Paper
	ShaderProgram m_MeshShader;
	ShaderProgram m_ParticleShader;

	// Mesh Object
	Mesh m_CubeMesh;
	Mesh m_Rabbit;
	Mesh m_Dragon;
	std::vector<GameObject> m_Objects;

	// VBO
	GLuint m_VBORect = 0;

	GLuint m_VBOCube = 0;

	std::vector<Vertex> m_Particles;
	std::vector<Vertex> m_ParticlesSSBO;

		//Particle VBO
	GLuint m_ParticleVBO;

	// Light
	DirectionalLight m_MainDirectionalLight;

	// Test
	float fTime = 0.0f;

	OctreeNode* m_pOctree;

	int m_particleCnt = 0;

	std::vector<ShaderStorageBufferObject*>	m_SSBO;
	ShaderProgram m_SSBOParticleShader;
	ShaderProgram m_ComputeShader;
	Texture* m_NodeTexture;
	glm::ivec2 m_TextureResolution = glm::ivec2(128, 128);
	std::vector<OctreeNode::Node> m_NodeBuffer;
	GLuint emptyVAO;


};

