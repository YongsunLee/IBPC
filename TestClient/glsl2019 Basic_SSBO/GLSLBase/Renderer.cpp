
#include "stdafx.h"
#include "Camera.h"
#include "GameObject.h"
#include "Renderer.h"
#include "ShaderStorageBufferObject.h"
#include "Octree.h"

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

	m_SSBOParticleShader.compile("./Shaders/SSBOParticle.vert", "./Shaders/SSBOParticle.geom", "./Shaders/SSBOParticle.frag");

	m_UpdateComputeShader.compile("./Shaders/UpdateComputeShader.comp");

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

	//CreateParticleVBO();

	CreateSSBO();
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

void Renderer::CreateSSBO()
{
	m_Root = BuildOctree(glm::vec3(0, 0, 0), 100, 1);

	m_particleCnt = 50000;
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> dir;
	std::vector<float> speed;
	for (int i = 0; i < m_particleCnt; i++) {
		Vertex p;
		positions.push_back(p.pos = glm::vec3(RAND_FLOAT(-50.0f, 50.0f), RAND_FLOAT(60.f, 120.0f), RAND_FLOAT(-50.0f, 50.0f)));
		dir.push_back(p.dir = glm::vec3(0, -1.0f, 0));
		speed.push_back(p.speed = RAND_FLOAT(0.0, 0.1f));

		m_Particles.push_back(p);
		m_Root->AddObject(&p);
	}

	m_SSBOs.push_back(new ShaderStorageBufferObject(GL_DYNAMIC_DRAW, positions));
	m_SSBOs.push_back(new ShaderStorageBufferObject(GL_DYNAMIC_DRAW, dir));
	m_SSBOs.push_back(new ShaderStorageBufferObject(GL_DYNAMIC_DRAW, speed));

	// Texture

	// Node Vectore
	std::vector<OctreeNode::Node> nodes;

	// Loop
	OctreeNode* curr;
	std::list<OctreeNode*> toProcess;
	toProcess.push_back(m_Root);

	glm::ivec2 pixel = glm::ivec2(0);
	glm::ivec2 childOffset = glm::ivec2(1, 0);
	glm::ivec2 TextureResolution = glm::ivec2(250, 250);
	int ArrayOffset = 1;

	glm::vec4 data = glm::vec4(0.0f);

	while (!toProcess.empty())
	{
		curr = toProcess.front();

		// 현재 픽셀좌표가 0이 아니면
		if (pixel.x != 0) {
			// 250 넘기면 맨 앞으로 보내려고
			pixel.y += int(pixel.x / TextureResolution.x);
			// x도 250 넘기면 맨앞으로 보내야지
			pixel.x = pixel.x % TextureResolution.x;
		}

		// 자식 좌표
		if (childOffset.x != 0) {
			childOffset.y += int(childOffset.x / TextureResolution.x);
			childOffset.x = childOffset.x % TextureResolution.x;
		}

		// 새로 노드 하나 생성
		OctreeNode::Node octToNode;
		// 옥트리 루트의 값을 노드로 변경해서 저장
		//octToNode.info.x = curr->triangleIDs.size();
		//octToNode.info.y = curr->GetChild().size();
		//octToNode.info.z = curr->isLeaf;
		//octToNode.info.w = curr->depth;
		//
		//// 현재 삼각형 ID 크기만큼
		//for (int i = 0; i < curr->triangleIDs.size(); i++) {
		//	// 새로 만든 노드의 Ref x에 해당 값을 넣어줌
		//	octToNode.triangleRefs[i].x = curr->triangleIDs[i];
		//}
		//// Box도 같은 형태로
		//octToNode.region = curr->region;
		//
		//// 노드 배열에 추가
		//nodes.push_back(octToNode);
		//
		//// 현재 삼각형 인덱스의 크기가 0보다 크면 == 자식 노드가 있으면
		//if (curr->trisIndices.size() > 0) {
		//
		//	// 현재 옥트리의 Active Child 갯수만큼
		//	for (int i = 0; i < curr->trisIndices.size(); i++) {
		//		// 반복문을 위한 to Process 추가
		//		toProcess.push_back(curr->children[curr->trisIndices[i]]);
		//	}
		//
		//	// 노드 data 세팅
		//	data = glm::vec4(childrenOffset.x, childrenOffset.y, curr->trisIndices.size(), oneDimensionalOffset); // Offset x, y, number of active children, and offset in the one-dimensional array.
		//
		//	// 1차원 배열 offset 8개씩 밀어서
		//	oneDimensionalOffset += curr->trisIndices.size();
		//
		//	// 자식 노드 오프셋도 8개 밀어서
		//	childrenOffset.x += curr->trisIndices.size();
		//}
		//else {
		//	// 인덱스가 0보다 작은경우 ( 리프 임 )
		//	data = glm::vec4(0, 0, 0, 0);
		//
		//	// 오프셋 1개
		//	oneDimensionalOffset += 1;
		//}
		//_offsetTexture->setData(glm::ivec2(1, 1), currPixel, &data);
		//currPixel.x += 1;
		toProcess.pop_front();
	}

	//m_SSBOs.push_back(new ShaderStorageBufferObject(GL_DYNAMIC_DRAW, nodes));
	//m_SSBOs.push_back(new ShaderStorageBufferObject(GL_DYNAMIC_DRAW, m_Particles));
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

void Renderer::DrawSystem()
{
	glEnable(GL_DEPTH_TEST);

	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	for (auto& obj : m_Objects)
		DrawObject(obj);
	
	//DrawParticle();
	Update();
	Draw();

	glDisable(GL_DEPTH_TEST);
}

void Renderer::Update()
{
	auto shader = m_UpdateComputeShader.get();
	glUseProgram(shader);

	fTime += g_tick;
	glUniform1f(glGetUniformLocation(shader, "u_Time"), fTime);





	for (int i = 0; i < 3; i++) {
		m_SSBOs[i]->bindBase(i);
	}
	glDispatchCompute((GLint)m_particleCnt / 128, 1, 1);
}

void Renderer::Draw()
{
	auto shader = m_SSBOParticleShader.get();
	glUseProgram(shader);

	if (nullptr != m_pCamera) {
		GLuint gProjViewId = glGetUniformLocation(shader, "u_ProjView");
		glUniformMatrix4fv(gProjViewId, 1, GL_FALSE, &m_pCamera->GetProjView()[0][0]);
	}

	glUniform1i(glGetUniformLocation(shader, "u_ClientWidth"), g_ClientWidth);
	glUniform1i(glGetUniformLocation(shader, "u_ClientHeight"), g_ClientHeight);
	auto& camera_pos = m_pCamera->GetPos();
	glUniform3f(glGetUniformLocation(shader, "u_CameraPos"), camera_pos.x, camera_pos.y, camera_pos.z);

	for (int i = 0; i < 3; i++) {
		m_SSBOs[i]->bindBase(i);
	}
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	glDrawArraysInstanced(GL_POINTS, 0, 1, m_particleCnt);
}	

OctreeNode* Renderer::BuildOctree(glm::vec3 vCenter, FLOAT fHalfWidth, int depthLimit)
{
	if (depthLimit < 0)
		return NULL;

	//현재 노드를 생성
	OctreeNode* pOctNode = new OctreeNode();
	pOctNode->SetPosition(vCenter);
	pOctNode->SetWidth(fHalfWidth);

	//재귀적으로 8개의 자식 노드들을 생성합니다.
	glm::vec3 vOffset;
	glm::vec3 vChildCenter;
	FLOAT fStep = fHalfWidth * 0.5f;

	//8개의 자식 노드들에 대해서 중심 위치를 설정하고 트리를 생성.
	for (int i = 0; i < 8; ++i) {

		vOffset[0] = ((i & 1) ? fStep : -fStep);
		vOffset[1] = ((i & 4) ? fStep : -fStep);
		vOffset[2] = ((i & 2) ? fStep : -fStep);

		vChildCenter[0] = vOffset[0] / (float)2 + vCenter[0];
		vChildCenter[1] = vOffset[1] / (float)2 + vCenter[1];
		vChildCenter[2] = vOffset[2] / (float)2 + vCenter[2];

		pOctNode->AddChildNode(BuildOctree(vChildCenter, fStep, depthLimit - 1));
	}

	if (depthLimit != 0)
		pOctNode->SetIsLeaf(false);

	return pOctNode;
}
