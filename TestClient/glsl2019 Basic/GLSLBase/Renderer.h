#pragma once

#include "Light.h"
#include "ShaderProgram.h"
#include "Mesh.h"

class Camera;
class GameObject;

class Renderer
{
public:
	Renderer(int windowSizeX, int windowSizeY);
	~Renderer();

	// Camera Set
	void SetCamera(Camera* pCamera) { m_pCamera = pCamera; };
	
	void Test();

	// Draw Mesh Object
	void DrawObject(GameObject& obj);
	
	// Draw Particle
	void DrawParticle();

	// Draw All Object
	void DrawSystem();

	void DrawParticleFountain();

private:
	void Initialize(int windowSizeX, int windowSizeY);
	void CreateVertexBufferObjects(); 

	void CreateSceneObject();
	void CreateParticleVBO();
	void CreateParticleFountainVBO();

private:
	bool m_Initialized = false;

	unsigned int m_ClientWidth = 0;
	unsigned int m_ClientHeight = 0;

	// Camera
	Camera* m_pCamera;

	// Shader
		// Test Shader
	ShaderProgram m_SolidRectShader;

		// Shader to Use Paper
	ShaderProgram m_MeshShader;
	ShaderProgram m_ParticleShader;

	ShaderProgram m_ParticleFountainShader;

	// Mesh Object
	Mesh m_CubeMesh;
	std::vector<GameObject> m_Objects;

	// VBO
	GLuint m_VBORect = 0;

		//Particle VBO
	GLuint m_ParticleVBO;
		
		//Fountain VBO
	GLuint m_ParticleFountainVBO;

	// Light
	DirectionalLight m_MainDirectionalLight;


	// Test
	float fTime = 0.0f;
};

