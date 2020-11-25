#pragma once
#include "Light.h"
#include "Framebuffer.h"
#include "ShaderProgram.h"
#include "Mesh.h"

class Camera;
class GameObject;

class Renderer {
public:
	struct VBO {
		GLuint buf;
		GLuint count;
		VBO() : buf(0), count(0) {}
	};
	struct TFO {
		GLuint tf;
		GLuint buf;
		GLuint count;
		TFO() : tf(0), buf(0), count(0) {}
	};

public:
	Renderer(unsigned int client_width, unsigned int client_height);
	~Renderer();

	void SetCamera(Camera* pCamera) { m_pCamera = pCamera; }
	void SetFixedCDCamera(Camera* pCamera) { m_pFixedCDCamera = pCamera; }
	void SetCDCamera(Camera* pCamera) { m_pCDCamera = pCamera; }

	void DrawParticleFixedCDTexture();
	void DrawParticleCDTexture();
	void DrawParticles();

	void DrawObjectFixedCDTexture(GameObject& obj);
	void DrawObjectCDTexture(GameObject& obj);
	void DrawObject(GameObject& obj);

	void DumpTexture(GLuint tex_id, GLuint x, GLuint y, GLuint w, GLuint h);

	void DrawParticleSystem();

	bool m_UpdateSwitch = false;

private:
	void Init(unsigned int client_width, unsigned int client_height);

	void CreateRectVBO(); 
	void CreateParticleTFO();

	void CreateSceneObjects();

	void CreateFrameResources();

private:
	bool						m_bInit;
	
	unsigned int				m_ClientWidth;
	unsigned int				m_ClientHeight;

	Camera*						m_pCamera;
	Camera*						m_pFixedCDCamera;
	Camera*						m_pCDCamera;

	VBO							m_vboRect;

	ShaderProgram				m_TextureDumpShader;
	ShaderProgram				m_ParticleShader;
	ShaderProgram				m_ParticleCDShader;

	GLuint						m_nTFOs;
	TFO*						m_pTFOs;
	int							m_CurrTFO;

	Framebuffer					m_FB_ParticleCDTexture[2];
	Framebuffer					m_FB_ObjectCDTexture;
	Framebuffer					m_SceneTexture;

	//Mesh Object
	ShaderProgram				m_MeshShader;
	ShaderProgram				m_MeshCDShader;

	Mesh						m_CubeMesh;
	Mesh						m_BunnyMesh;
	Mesh						m_DragonMesh;

	std::vector<GameObject>		m_Objects;

	// Light
	DirectionalLight m_MainDirectionalLight;

};

