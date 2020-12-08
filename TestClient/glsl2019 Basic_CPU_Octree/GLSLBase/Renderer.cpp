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
	m_SolidRectShader.compile("./Shaders/SolidRect.vert", "./Shaders/SolidRect.frag");
	m_MeshShader.compile("./Shaders/SolidMesh.vert", "./Shaders/SolidMesh.frag");
	m_ParticleShader.compile("./Shaders/Particle.vert", "./Shaders/Particle.geom", "./Shaders/Particle.frag");
	//m_ParticleShader.compile("./Shaders/Particle.vert", "./Shaders/Particle.frag");

	m_CubeShader.compile("./Shaders/BoundingBox.vert", "./Shaders/BoundingBox.frag");

	// Create Mesh Data
	m_CubeMesh.load("./Resource/Model/LightingCheckBoard_smooth.fbx");

	// Octree
	m_pOctree = new OctreeNode();	
	m_pOctree = BuildOctree(glm::vec3(0, 0, 0), 50, 3);

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

	float temp = 0.5f;

	float cube[] = {
		-temp, -temp,  temp, 0.f, 0.f, 1.f, 0.f, 1.f, 0.f, 1.f, //x, y, z, nx, ny, nz, r, g, b, a
		 temp,  temp,  temp, 0.f, 0.f, 1.f, 0.f, 1.f, 0.f, 1.f,
		-temp,  temp,  temp, 0.f, 0.f, 1.f, 0.f, 1.f, 0.f, 1.f,
		-temp, -temp,  temp, 0.f, 0.f, 1.f, 0.f, 1.f, 0.f, 1.f,
		 temp, -temp,  temp, 0.f, 0.f, 1.f, 0.f, 1.f, 0.f, 1.f,
		 temp,  temp,  temp, 0.f, 0.f, 1.f, 0.f, 1.f, 0.f, 1.f, // first face : R

		 temp, -temp,  temp, 1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 1.f, //x, y, z, nx, ny, nz, r, g, b, a
		 temp,  temp, -temp, 1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 1.f,
		 temp,  temp,  temp, 1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 1.f,
		 temp, -temp,  temp, 1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 1.f, //x, y, z, nx, ny, nz, r, g, b, a
		 temp, -temp, -temp, 1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 1.f,
		 temp,  temp, -temp, 1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 1.f, //second face : G

		-temp,  temp,  temp, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 1.f, //x, y, z, nx, ny, nz, r, g, b, a
		 temp,  temp, -temp, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 1.f,
		-temp,  temp, -temp, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 1.f,
		-temp,  temp,  temp, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 1.f, //x, y, z, nx, ny, nz, r, g, b, a
		 temp,  temp,  temp, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 1.f,
		 temp,  temp, -temp, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 1.f, //third face : B

		-temp, -temp, -temp, 0.f, 0.f, -1.f, 0.f, 1.f, 0.f, 1.f, //x, y, z, nx, ny, nz, r, g, b, a
		-temp,  temp, -temp, 0.f, 0.f, -1.f, 0.f, 1.f, 0.f, 1.f,
		 temp,  temp, -temp, 0.f, 0.f, -1.f, 0.f, 1.f, 0.f, 1.f,
		-temp, -temp, -temp, 0.f, 0.f, -1.f, 0.f, 1.f, 0.f, 1.f, //x, y, z, nx, ny, nz, r, g, b, a
		 temp,  temp, -temp, 0.f, 0.f, -1.f, 0.f, 1.f, 0.f, 1.f,
		 temp, -temp, -temp, 0.f, 0.f, -1.f, 0.f, 1.f, 0.f, 1.f, //fourth face : R+G (yellow)

		-temp, -temp,  temp, -1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 1.f, //x, y, z, nx, ny, nz, r, g, b, a 
		-temp,  temp,  temp, -1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 1.f,
		-temp,  temp, -temp, -1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 1.f,
		-temp, -temp,  temp, -1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 1.f, //x, y, z, nx, ny, nz, r, g, b, a 
		-temp,  temp, -temp, -1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 1.f,
		-temp, -temp, -temp, -1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 1.f, // fifth face : R+B (purple)

		-temp, -temp,  temp, 0.f, -1.f, 0.f, 0.f, 1.f, 0.f, 1.f, //x, y, z, nx, ny, nz, r, g, b, a 
		 temp, -temp, -temp, 0.f, -1.f, 0.f, 0.f, 1.f, 0.f, 1.f,
		 temp, -temp,  temp, 0.f, -1.f, 0.f, 0.f, 1.f, 0.f, 1.f,
		-temp, -temp,  temp, 0.f, -1.f, 0.f, 0.f, 1.f, 0.f, 1.f, //x, y, z, nx, ny, nz, r, g, b, a 
		-temp, -temp, -temp, 0.f, -1.f, 0.f, 0.f, 1.f, 0.f, 1.f,
		 temp, -temp, -temp, 0.f, -1.f, 0.f, 0.f, 1.f, 0.f, 1.f, //sixth face : G+B (Cyan)
	};

	glGenBuffers(1, &m_VBOCube);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOCube);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);

	CreateParticleVBO();
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
	int cnt = 1024;
	
	Vertex vertex;
	for (int j = 0; j < cnt; ++j) {
		vertex.pos = glm::vec3(RAND_FLOAT(-25.0f, 25.0f), RAND_FLOAT(40.f, 80.f), RAND_FLOAT(-25.0f, 25.0f));
		vertex.dir = glm::vec3(0, -1.0f, 0);
		vertex.speed = RAND_FLOAT(2.0, 5.0f);
		m_Particles.push_back(vertex);

		if(m_pOctree->IsInNode(vertex.pos)) 
			m_pOctree->AddObject(&vertex);
	}

	glGenBuffers(1, &m_ParticleVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * cnt, m_Particles.data(), GL_DYNAMIC_DRAW);
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

void Renderer::DrawCube()
{
	GLuint shader = m_CubeShader.get();

	glUseProgram(shader);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glUniformMatrix4fv(glGetUniformLocation(shader, "u_ProjView"), 1, GL_FALSE, &m_pCamera->GetProjView()[0][0]);

	float width = 10.0f;
	glm::mat4 CubeTransform = glm::scale(glm::mat4(1), glm::vec3(width));
	glUniformMatrix4fv(glGetUniformLocation(shader, "u_Transform"), 1, GL_FALSE, &CubeTransform[0][0]);

	int attribPosition = glGetAttribLocation(shader, "a_Position");
	int attribNormal = glGetAttribLocation(shader, "a_Normal");
	int attribColor = glGetAttribLocation(shader, "a_Color");

	glEnableVertexAttribArray(attribPosition);
	glEnableVertexAttribArray(attribNormal);
	glEnableVertexAttribArray(attribColor);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBOCube);

	glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 10, 0);
	glVertexAttribPointer(attribNormal, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 10, (GLvoid*)(sizeof(float) * 3));
	glVertexAttribPointer(attribColor, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 10, (GLvoid*)(sizeof(float) * 6));

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glDisableVertexAttribArray(attribPosition);
	glDisableVertexAttribArray(attribNormal);
	glDisableVertexAttribArray(attribColor);
}

void Renderer::DrawCube(glm::vec3 pos, float width)
{
	GLuint shader = m_CubeShader.get();

	glUseProgram(shader);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glUniformMatrix4fv(glGetUniformLocation(shader, "u_ProjView"), 1, GL_FALSE, &m_pCamera->GetProjView()[0][0]);

	glm::mat4 CubeTransform = glm::translate(glm::mat4(1), pos) * glm::scale(glm::mat4(1), glm::vec3(width));
	glUniformMatrix4fv(glGetUniformLocation(shader, "u_Transform"), 1, GL_FALSE, &CubeTransform[0][0]);

	int attribPosition = glGetAttribLocation(shader, "a_Position");
	int attribNormal = glGetAttribLocation(shader, "a_Normal");
	int attribColor = glGetAttribLocation(shader, "a_Color");

	glEnableVertexAttribArray(attribPosition);
	glEnableVertexAttribArray(attribNormal);
	glEnableVertexAttribArray(attribColor);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBOCube);

	glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 10, 0);
	glVertexAttribPointer(attribNormal, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 10, (GLvoid*)(sizeof(float) * 3));
	glVertexAttribPointer(attribColor, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 10, (GLvoid*)(sizeof(float) * 6));

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glDisableVertexAttribArray(attribPosition);
	glDisableVertexAttribArray(attribNormal);
	glDisableVertexAttribArray(attribColor);
}

void Renderer::DrawCube(GameObject& obj)
{
	GLuint shader = m_CubeShader.get();

	glUseProgram(shader);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glUniformMatrix4fv(glGetUniformLocation(shader, "u_ProjView"), 1, GL_FALSE, &m_pCamera->GetProjView()[0][0]);
	
	glm::mat4 CubeTransform = obj.transform().mTransform * obj.mesh()->GetAABBTransform();
	glUniformMatrix4fv(glGetUniformLocation(shader, "u_Transform"), 1, GL_FALSE, &CubeTransform[0][0]);

	int attribPosition = glGetAttribLocation(shader, "a_Position");
	int attribNormal = glGetAttribLocation(shader, "a_Normal");
	int attribColor = glGetAttribLocation(shader, "a_Color");

	glEnableVertexAttribArray(attribPosition);
	glEnableVertexAttribArray(attribNormal);
	glEnableVertexAttribArray(attribColor);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBOCube);

	glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 10, 0);
	glVertexAttribPointer(attribNormal, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 10, (GLvoid*)(sizeof(float) * 3));
	glVertexAttribPointer(attribColor, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 10, (GLvoid*)(sizeof(float) * 6));

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glDisableVertexAttribArray(attribPosition);
	glDisableVertexAttribArray(attribNormal);
	glDisableVertexAttribArray(attribColor);
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

	//DrawCube(obj);
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

	//fTime += g_tick;

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

	glDrawArrays(GL_POINTS, 0, 50000);

	glDisableVertexAttribArray(aPos);
	glDisableVertexAttribArray(aDir);
	glDisableVertexAttribArray(aSpeed);
	glDisableVertexAttribArray(aCollideTime);
}

void Renderer::UpdateOctree()
{
	// all tree Object Clear
	m_pOctree->Rebuild();
	
	for (auto p : m_Particles)
	{
		// CPU Particle Position Update
		p.pos = p.pos + ((p.dir * p.speed) + (glm::vec3(0, -1, 0) * fTime * p.speed));
	
		if (m_pOctree->IsInNode(p.pos))
			m_pOctree->AddObject(&p);
	}
}

void Renderer::DrawOctreee()
{
	OctreeNode* curr;
	std::list<OctreeNode*> toProcess;
	toProcess.push_back(m_pOctree);
	while (!toProcess.empty())
	{
		curr = toProcess.front();
		if(curr->GetObjectArray().size() != 0)
			DrawCube(curr->GetPos(), curr->GetWidth());
	
		if (curr->GetChild()[0] == nullptr) break;
		else
		{
			for (int i = 0; i < 8; ++i) {
	
				if (curr->GetChildNode(i) != nullptr)
				{
	
					if (curr->GetChildNode(i)->GetObjectArray().size() != 0)
					{
						//printf("%d\n", curr->GetChildNode(i)->GetObjectArray().size());
						//printf("%.f, %.f, %.f, %.f\n", curr->GetChildNode(i)->GetPos().x, curr->GetChildNode(i)->GetPos().y, curr->GetChildNode(i)->GetPos().z, curr->GetChildNode(i)->GetWidth());
						DrawCube(curr->GetChildNode(i)->GetPos(), curr->GetChildNode(i)->GetWidth());
					}
	
					toProcess.push_back(curr->GetChildNode(i));
				}
			}
		}
	
		toProcess.pop_front();
	}
}

void Renderer::DrawSystem()
{
	glEnable(GL_DEPTH_TEST);

	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	//for (auto& obj : m_Objects)
	//{
	//	DrawObject(obj);
	//}
	fTime += g_tick;

	DrawParticle();
	DrawOctreee();

	UpdateOctree();

	glDisable(GL_DEPTH_TEST);
}



OctreeNode* Renderer::BuildOctree(glm::vec3 vCenter, FLOAT fHalfWidth, int depthLimit)
{
	if (depthLimit < 0)
		return NULL;

	//현재 노드를 생성
	OctreeNode* pOctNode = new OctreeNode();
	pOctNode->SetPosition(vCenter);
	pOctNode->SetWidth(fHalfWidth);
	pOctNode->SetDepth(depthLimit);

	glm::vec3 aabbMin;
	glm::vec3 aabbMax;

	aabbMin = vCenter - glm::vec3(fHalfWidth);
	aabbMax = vCenter + glm::vec3(fHalfWidth);

	//재귀적으로 8개의 자식 노드들을 생성합니다.
	glm::vec3 vOffset;
	glm::vec3 vChildCenter;
	FLOAT fStep = fHalfWidth * 0.5f;

	//8개의 자식 노드들에 대해서 중심 위치를 설정하고 트리를 생성.
	for (int i = 0; i < 8; ++i) {
		
		vOffset[0] = ((i & 1) ? fStep : -fStep);
		vOffset[1] = ((i & 4) ? fStep : -fStep);
		vOffset[2] = ((i & 2) ? fStep : -fStep);

		vChildCenter[0] = (vOffset[0] / (float)2) + vCenter[0];
		vChildCenter[1] = (vOffset[1] / (float)2) + vCenter[1];
		vChildCenter[2] = (vOffset[2] / (float)2) + vCenter[2];

		pOctNode->AddChildNode(BuildOctree(vChildCenter, fStep, depthLimit - 1));
	}

	if (depthLimit == 0)
		pOctNode->SetIsLeaf(true);

	return pOctNode;
}
