#include "stdafx.h"
#include "Camera.h"
#include "GameObject.h"
#include "Renderer.h"

const float g_tick = 0.0166666666666667f;

Renderer::Renderer(int windowSizeX, int windowSizeY)
{
	Initialize(windowSizeX, windowSizeY);
}


Renderer::~Renderer()
{
}

void Renderer::Initialize(int windowSizeX, int windowSizeY)
{
	//Set window size
	m_ClientWidth = windowSizeX;
	m_ClientHeight = windowSizeY;

	//Load shaders
	m_SolidRectShader.compile("./Shaders/SolidRect.vs", "./Shaders/SolidRect.fs");
	m_MeshShader.compile("./Shaders/SolidMesh.vert", "./Shaders/SolidMesh.frag");
	m_ParticleShader.compile("./Shaders/Particle.vert", "./Shaders/Particle.geom", "./Shaders/Particle.frag");
	//m_ParticleShader.compile("./Shaders/Particle.vert", "./Shaders/Particle.frag");
	m_ParticleFountainShader.compile("./Shaders/ParticleFountain.vert", "./Shaders/ParticleFountain.geom", "./Shaders/ParticleFountain.frag");

	// Create Mesh Data
	m_CubeMesh.load("./Resource/Model/LightingCheckBoard_smooth.fbx");

	//Create VBOs
	CreateVertexBufferObjects();

	// Light
	m_MainDirectionalLight.mLightColor = glm::vec4(255.f, 244.f, 214.f, 255.f);
	m_MainDirectionalLight.mLightColor /= 255.f;
	m_MainDirectionalLight.mDirection = glm::normalize(glm::vec3(25, -30, 0));

	// Objects
	CreateSceneObject();
}

void Renderer::CreateVertexBufferObjects()
{
	float rect[]
		=
	{
		-0.5, -0.5, 0.f, -0.5, 0.5, 0.f, 0.5, 0.5, 0.f, //Triangle1
		-0.5, -0.5, 0.f,  0.5, 0.5, 0.f, 0.5, -0.5, 0.f, //Triangle2
	};

	glGenBuffers(1, &m_VBORect);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBORect);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rect), rect, GL_STATIC_DRAW);

	CreateParticleVBO();
	CreateParticleFountainVBO();
}

void Renderer::CreateSceneObject()
{
	m_Objects.emplace_back();
	auto& board = m_Objects.back();
	board.set_mesh(&m_CubeMesh);
	board.transform().set_pos(glm::vec3(0.0f, -0.5f, 0.0f));
	board.transform().set_scale(glm::vec3(1.f));
}

void Renderer::CreateParticleVBO()
{
	int cnt = 50000;
	std::vector<Vertex> particles;

	Vertex vertex;
	for (int j = 0; j < cnt; ++j) {
		vertex.pos = glm::vec3(RAND_FLOAT(-50.0f, 50.0f), RAND_FLOAT(40.f, 80.f), RAND_FLOAT(-50.0f, 50.0f));
		vertex.dir = glm::vec3(0, -1.0f, 0);
		vertex.speed = RAND_FLOAT(1.0f, 5.0f);
		particles.push_back(vertex);
	}

	glGenBuffers(1, &m_ParticleVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * cnt, particles.data(), GL_DYNAMIC_DRAW);
}


void Renderer::CreateParticleFountainVBO()
{
	int cnt = 50000;
	std::vector<Vertex> particles;

	Vertex vertex;
	for (int j = 0; j < cnt/2; ++j) {
		vertex.pos = glm::vec3(RAND_FLOAT(-1.0f, 0.0f), 20, RAND_FLOAT(-1.0f, 1.0f));
		//vertex.dir = glm::vec3(RAND_FLOAT(-1, 1), RAND_FLOAT(1, 5), RAND_FLOAT(-1, 1));
		vertex.dir = glm::vec3(1, 0, 0);
		vertex.speed = 1.0f;;
		particles.push_back(vertex);
	}

	for (int i = cnt / 2; i < cnt; ++i) {
		vertex.pos = glm::vec3(RAND_FLOAT(0.0f, 1.0f), 20, RAND_FLOAT(-1.0f, 1.0f));
		//vertex.dir = glm::vec3(RAND_FLOAT(-1, 1), RAND_FLOAT(1, 5), RAND_FLOAT(-1, 1));
		vertex.dir = glm::vec3(-1, 0, 0);
		vertex.speed = 1.0f;;
		particles.push_back(vertex);
	}

	glGenBuffers(1, &m_ParticleFountainVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleFountainVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * cnt, particles.data(), GL_DYNAMIC_DRAW);
}

void Renderer::Test()
{
	auto shader = m_SolidRectShader.get();
	
	glUseProgram(shader);

	int attribPosition = glGetAttribLocation(shader, "a_Position");
	glEnableVertexAttribArray(attribPosition);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBORect);
	glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(attribPosition);
}

void Renderer::DrawObject(GameObject& obj)
{
	auto shader = m_MeshShader.get();

	glUseProgram(shader);

	glUniformMatrix4fv(glGetUniformLocation(shader, "u_ProjView"), 1, GL_FALSE, &m_pCamera->GetProjView()[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader, "u_World"), 1, GL_FALSE, &obj.transform().mtx()[0][0]);
	auto& camera_pos = m_pCamera->GetPos();
	glUniform3f(glGetUniformLocation(shader, "u_CameraPos"), camera_pos.x, camera_pos.y, camera_pos.z);

	glUniform4f(
		glGetUniformLocation(shader, "dirLight.mLightColor")
		, m_MainDirectionalLight.mLightColor.x
		, m_MainDirectionalLight.mLightColor.y
		, m_MainDirectionalLight.mLightColor.z
		, m_MainDirectionalLight.mLightColor.w);

	glUniform3f(
		glGetUniformLocation(shader, "dirLight.mDirection")
		, m_MainDirectionalLight.mDirection.x
		, m_MainDirectionalLight.mDirection.y
		, m_MainDirectionalLight.mDirection.z);

	int attribPosition = glGetAttribLocation(shader, "a_Position");
	int attribNormal = glGetAttribLocation(shader, "a_Normal");
	int attribTexCoord = glGetAttribLocation(shader, "a_TexCoord");

	glEnableVertexAttribArray(attribPosition);
	glEnableVertexAttribArray(attribNormal);
	glEnableVertexAttribArray(attribTexCoord);

	obj.bind_resources();

	glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(attribNormal, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(float) * 3));
	glVertexAttribPointer(attribTexCoord, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(float) * 6));

	obj.render();

	glDisableVertexAttribArray(attribPosition);
	glDisableVertexAttribArray(attribNormal);
	glDisableVertexAttribArray(attribTexCoord);
}

void Renderer::DrawParticle()
{
	auto shader = m_ParticleShader.get();
	glUseProgram(shader);

	if (nullptr != m_pCamera) {
		GLuint gProjViewId = glGetUniformLocation(shader, "u_ProjView");
		glUniformMatrix4fv(gProjViewId, 1, GL_FALSE, &m_pCamera->GetProjView()[0][0]);
	}

	glUniform1i(glGetUniformLocation(shader, "u_ClientWidth"), g_ClientWidth);
	glUniform1i(glGetUniformLocation(shader, "u_ClientHeight"), g_ClientHeight);
	auto& camera_pos = m_pCamera->GetPos();
	glUniform3f(glGetUniformLocation(shader, "u_CameraPos"), camera_pos.x, camera_pos.y, camera_pos.z);

	fTime += g_tick;

	glUniform1f(glGetUniformLocation(shader, "u_Time"), fTime);

	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleVBO);

	GLint aPos = glGetAttribLocation(shader, "a_Pos");
	GLint aDir = glGetAttribLocation(shader, "a_Dir");
	GLint aSpeed = glGetAttribLocation(shader, "a_Speed");
	GLint aCollideTime = glGetAttribLocation(shader, "a_CollideTime");

	glEnableVertexAttribArray(aPos);
	glEnableVertexAttribArray(aDir);
	glEnableVertexAttribArray(aSpeed);
	glEnableVertexAttribArray(aCollideTime);

	glVertexAttribPointer(aPos, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(aDir, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(float) * 3));
	glVertexAttribPointer(aSpeed, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(float) * 6));
	glVertexAttribPointer(aCollideTime, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(float) * 7));

	glPointSize(3.0f);
	glDrawArrays(GL_POINTS, 0, 50000);

	glDisableVertexAttribArray(aPos);
	glDisableVertexAttribArray(aDir);
	glDisableVertexAttribArray(aSpeed);
	glDisableVertexAttribArray(aCollideTime);
}

void Renderer::DrawParticleFountain()
{
	auto shader = m_ParticleFountainShader.get();
	glUseProgram(shader);

	if (nullptr != m_pCamera) {
		GLuint gProjViewId = glGetUniformLocation(shader, "u_ProjView");
		glUniformMatrix4fv(gProjViewId, 1, GL_FALSE, &m_pCamera->GetProjView()[0][0]);
	}

	glUniform1i(glGetUniformLocation(shader, "u_ClientWidth"), g_ClientWidth);
	glUniform1i(glGetUniformLocation(shader, "u_ClientHeight"), g_ClientHeight);
	auto& camera_pos = m_pCamera->GetPos();
	glUniform3f(glGetUniformLocation(shader, "u_CameraPos"), camera_pos.x, camera_pos.y, camera_pos.z);

	fTime += g_tick;

	glUniform1f(glGetUniformLocation(shader, "u_Time"), fTime);

	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleFountainVBO);

	GLint aPos = glGetAttribLocation(shader, "a_Pos");
	GLint aDir = glGetAttribLocation(shader, "a_Dir");
	GLint aSpeed = glGetAttribLocation(shader, "a_Speed");
	GLint aCollideTime = glGetAttribLocation(shader, "a_CollideTime");

	glEnableVertexAttribArray(aPos);
	glEnableVertexAttribArray(aDir);
	glEnableVertexAttribArray(aSpeed);
	glEnableVertexAttribArray(aCollideTime);

	glVertexAttribPointer(aPos, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(aDir, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(float) * 3));
	glVertexAttribPointer(aSpeed, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(float) * 6));
	glVertexAttribPointer(aCollideTime, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(float) * 7));

	glPointSize(3.0f);
	glDrawArrays(GL_POINTS, 0, 50000);

	glDisableVertexAttribArray(aPos);
	glDisableVertexAttribArray(aDir);
	glDisableVertexAttribArray(aSpeed);
	glDisableVertexAttribArray(aCollideTime);
}

void Renderer::DrawSystem()
{
	glEnable(GL_DEPTH_TEST);

	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	for (auto& obj : m_Objects)
		DrawObject(obj);
	//DrawParticle();

	DrawParticleFountain();

	glDisable(GL_DEPTH_TEST);
}
