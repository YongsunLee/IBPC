#include "stdafx.h"
#include "Camera.h"
#include "GameObject.h"
#include "Renderer.h"

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
	m_SSBOParticleShader.compile("./Shaders/SSBOParticle.vert", "./Shaders/SSBOParticle.geom", "./Shaders/SSBOParticle.frag");

	m_ComputeShader.compile("./Shaders/UpdateComputeShader.comp");
	m_CubeShader.compile("./Shaders/BoundingBox.vert", "./Shaders/BoundingBox.frag");

	// Create Mesh Data
	m_CubeMesh.load("./Resource/Model/Cube.fbx");
	m_Rabbit.load("./Resource/Model/TinyBunny.fbx");
	m_Dragon.load("./Resource/Model/TinyDragonOptimized.fbx");

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

	//CreateParticleVBO();
	//CreateParticleSSBOs();
	CreateParticleSSBO();
}

void Renderer::CreateSceneObject()
{
	m_Objects.emplace_back();
	auto& board = m_Objects.back();
	board.set_mesh(&m_CubeMesh);
	board.transform().set_pos(glm::vec3(0.0f, -30.0, 0.0f));
	//board.transform().set_scale(glm::vec3(1.f));
	board.transform().set_scale(glm::vec3(1.f, 0.1f, 1.f));
	m_Objects.emplace_back();
	
	//auto& bunny = m_Objects.back();
	//bunny.set_mesh(&m_Rabbit);
	//bunny.transform().set_pos(glm::vec3(0.0f, -25.0, 0));
	//bunny.transform().set_scale(glm::vec3(5.0f));
	//m_Objects.emplace_back();
	
	auto& dragon = m_Objects.back();
	dragon.set_mesh(&m_Dragon);
	dragon.transform().set_pos(glm::vec3(0.0f, -25.0, 0));
	dragon.transform().set_scale(glm::vec3(5.0f));
	dragon.transform().set_angle(glm::vec3(0.0f, 0.0f, 1.0f));
}

void Renderer::CreateParticleSSBO()
{
	m_particleCnt = 50000;

	Vertex vertex;
	// 떨어지는 형태
	//for (int j = 0; j < m_particleCnt; ++j) {
	//	//vertex.pos = glm::vec3(RAND_FLOAT(-25.0f, 25.0f), RAND_FLOAT(20.0, 20.f), RAND_FLOAT(-25.0f, 25.0f));
	//	vertex.pos = glm::vec3(RAND_FLOAT(-25.0f, 25.0f), RAND_FLOAT(40.0, 80.f), RAND_FLOAT(-25.0f, 25.0f));
	//	//vertex.pos = glm::vec3(RAND_FLOAT(-25.0f, 25.0f), 20.0f, 0.0f);
	//
	//	//printf("pos : %.f, %.f, %.f \n", vertex.pos.x, vertex.pos.y, vertex.pos.z);
	//
	//	vertex.dir = glm::vec3(0, -1.0f, 0);
	//	vertex.speed = RAND_FLOAT(1.0f, 2.0f);
	//	vertex.collide_time = 0.0f;
	//
	//	m_ParticlesSSBO.push_back(vertex);
	//
	//	if (m_pOctree->IsInNode(vertex.pos))
	//		m_pOctree->AddObject(&vertex, j);
	//}

	// 좌우에서 오는 형태
	for (int i = 0; i < m_particleCnt / 2; i++) {
		//vertex.pos = glm::vec3(RAND_FLOAT(-25, -15), RAND_FLOAT(-1, 1), RAND_FLOAT(-1, 1));
		vertex.pos = glm::vec3(RAND_FLOAT(-75, -25.0f), RAND_FLOAT(-25, 25.f), RAND_FLOAT(-25, 25.0f));
		//vertex.pos = glm::vec3(RAND_FLOAT(0.0f, 30.0f), RAND_FLOAT(-15, 15), RAND_FLOAT(-15, 15));
		//vertex.pos = glm::vec3( 30.0f, RAND_FLOAT(-15, 15), 15.f);
		vertex.dir = glm::vec3(1.0f, 0.0f, 0);
		vertex.speed = RAND_FLOAT(0.05f, 0.05f);
		vertex.collide_time = 0.0f;
	
		m_ParticlesSSBO.push_back(vertex);
		
		if (m_pOctree->IsInNode(vertex.pos))
			m_pOctree->AddObject(&vertex, i);
	}
	
	for (int i = m_particleCnt / 2; i < m_particleCnt; i++) {
		//vertex.pos = glm::vec3(RAND_FLOAT(15, 25), RAND_FLOAT(-1, 1), RAND_FLOAT(-1, 1));
		//vertex.pos = glm::vec3(RAND_FLOAT(50, 75), RAND_FLOAT(-25, 25), RAND_FLOAT(-25, 25));
		vertex.pos = glm::vec3(RAND_FLOAT(25, 75.0f), RAND_FLOAT(-25, 25.f), RAND_FLOAT(-25, 25.0f));
		//vertex.pos = glm::vec3(-30.f, RAND_FLOAT(-15, 15), 15.f);
		vertex.dir = glm::vec3(-1.0f, 0.0f, 0);
		vertex.speed = RAND_FLOAT(0.05f, 0.05f);
		vertex.collide_time = 0.0f;
	
		m_ParticlesSSBO.push_back(vertex);
	
		if (m_pOctree->IsInNode(vertex.pos))
			m_pOctree->AddObject(&vertex, i);
	}

	//printf("----------init-------------\n");

	m_SSBO.push_back(new ShaderStorageBufferObject(GL_DYNAMIC_DRAW, m_ParticlesSSBO));

	// Texture
	m_NodeTexture = new Texture(Texture::RGBA16f, m_TextureResolution);
	
	// Loop
	OctreeNode* curr;
	std::list<OctreeNode*> toProcess;
	toProcess.push_back(m_pOctree);
	
	glm::ivec2 pixel = glm::ivec2(0);
	glm::ivec2 childOffset = glm::ivec2(1, 0);
	int NodeArrayOffset = 1;
	
	glm::vec4 data = glm::vec4(0.0f);
	
	int pCnt = 0;
	while (!toProcess.empty())
	{
		curr = toProcess.front();
	
		// 현재 픽셀좌표가 0이 아니면
		// x가 Texture 해상도를 넘기면 y가 하나 내려감
		// x가 Texture 해상도를 넘기면 0으로 돌아감
		if (pixel.x != 0) {
			pixel.y += int(pixel.x / m_TextureResolution.x);
			pixel.x = pixel.x % m_TextureResolution.x;
		}
	
		// 자식 좌표
		if (childOffset.x != 0) {
			childOffset.y += int(childOffset.x / m_TextureResolution.x);
			childOffset.x = childOffset.x % m_TextureResolution.x;
		}
	
		// 새로 노드 하나 생성
		OctreeNode::Node octToNode;
	
		//현재 파티클이 이 트리에 포함되는가
		//이 트리가 자식노드를 가지는가
		//-> Yes 자식 탐색
		//-> No 파티클 충돌 준비 (몇개의 파티클을 가지고 있는가, Ref 저장)
		//
		//자식노드 개수, 파티클 개수, 파티클 Ref 저장
		//
		//옥트리 루트의 값을 노드로 변경해서 저장
		octToNode.info.x = curr->GetObejctIDs().size();	// Size는 파티클의 개수를 의미
		octToNode.info.y = curr->GetChild().size();		// 자식개수
		octToNode.info.z = curr->GetIsLeaf();			// Leaf 노드인가?
		octToNode.info.w = curr->GetDepth();			// depth
	
		//printf("isLeaf: %i, depth: %i, stored at pixel: (%i, %i) with number of Particles: %d\n", 
		//	    octToNode.info.z, octToNode.info.w, pixel.x, pixel.y, octToNode.info.x);
	
		//pCnt += curr->GetObejctIDs().size();
	
		// 현재 파티클 개수만큼
		for (int i = 0; i < curr->GetObejctIDs().size(); ++i) {
			// Ref 저장
			octToNode.vertexRef[i].x = curr->GetObejctIDs()[i];
		}
		//BBox 용 Pos, Radius
		octToNode.aabb.x = curr->GetPos().x;
		octToNode.aabb.y = curr->GetPos().y;
		octToNode.aabb.z = curr->GetPos().z;
		octToNode.aabb.w = curr->GetWidth();
	
		// 노드 배열에 추가
		m_NodeBuffer.push_back(octToNode);
	
		// 이 노드를 가리키는 data를 텍스처에 저장해야함
		// 자식노드가 nullptr이면
		if (curr->GetChildNode(0) == nullptr)
		{
			data = glm::vec4(0, 0, 0, 0);
		}
		else
		{
			for (int i = 0; i < 8; ++i) {
				toProcess.push_back(curr->GetChildNode(i));
			}
	
			// 노드 data 세팅
			// Offset x, y, number of active children, and offset in the one-dimensional array.
			data = glm::vec4(childOffset.x, childOffset.y, 8, NodeArrayOffset);
	
			// 1차원 배열 offset 8개씩 밀어서
			NodeArrayOffset += 8;
	
			// 자식 노드 오프셋도 8개 밀어서
			childOffset.x += 8;
		}
		m_NodeTexture->setData(glm::ivec2(1, 1), pixel, &data);
		pixel.x += 1;
		toProcess.pop_front();
	}
	//printf("%d\n", pCnt);
	//printf("---init---\n");

	m_SSBO.push_back(new ShaderStorageBufferObject(GL_DYNAMIC_DRAW, m_NodeBuffer));
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

void Renderer::UpdateSSBO()
{
	auto shader = m_ComputeShader.get();
	glUseProgram(shader);

	glUniform1i(glGetUniformLocation(shader, "u_NodeTexture"), 0);
	m_NodeTexture->bind(0);

	for (int i = 0 ; i < 2; ++i)
	{
		m_SSBO[i]->bindBase(i);
	}

	//glDispatchCompute((GLint)m_particleCnt / 10, 1, 1);
	glDispatchCompute((GLint)m_particleCnt / 128, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void Renderer::DrawSSBOParticle()
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

	m_SSBO[0]->bindBase(0);
	//glBindVertexArray(emptyVAO);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	//glDrawArraysInstanced(GL_POINTS, 0, 1, m_particleCnt);
	glDrawArrays(GL_POINTS, 0, m_particleCnt);
	//glBindVertexArray(0);
}

void Renderer::UpdateNodeTexture()
{
	// Loop
	OctreeNode* curr;
	std::list<OctreeNode*> toProcess;
	toProcess.push_back(m_pOctree);

	int pCnt = 0;
	int nodeCnt = 0;
	while (!toProcess.empty())
	{
		curr = toProcess.front();

		m_NodeBuffer[nodeCnt].info.x = curr->GetObejctIDs().size();	// Size는 파티클의 개수를 의미
		m_NodeBuffer[nodeCnt].info.y = curr->GetChild().size();		// 자식개수
		m_NodeBuffer[nodeCnt].info.z = curr->GetIsLeaf();			// Leaf 노드인가?
		m_NodeBuffer[nodeCnt].info.w = curr->GetDepth();			// depth
		
		//pCnt += m_NodeBUffer[nodeCnt].info.x;

		// 현재 파티클 개수만큼
		for (int i = 0; i < curr->GetObejctIDs().size(); ++i) {
			// Ref 저장
			m_NodeBuffer[nodeCnt].vertexRef[i].x = curr->GetObejctIDs()[i];
		}

		// 이 노드를 가리키는 data를 텍스처에 저장해야함
		// 자식노드가 nullptr이면
		if (curr->GetChildNode(0) != nullptr)
		{
			for (int i = 0; i < 8; ++i) {
				toProcess.push_back(curr->GetChildNode(i));
			}
		}
		nodeCnt += 1;
		toProcess.pop_front();
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBO[1]->GetBuffer());
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_NodeBuffer.size() * sizeof(OctreeNode::Node), &m_NodeBuffer[0]);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Renderer::UpdateOctree()
{
	// all tree Object Clear
	m_pOctree->Rebuild();

	// GPU Update Data Load CPU
	glGetNamedBufferSubData(m_SSBO[0]->GetBuffer(), 0, m_particleCnt * sizeof(Vertex), m_ParticlesSSBO.data());

	int idx = 0;
	for (auto p : m_ParticlesSSBO)
	{
		//printf("pos : %.f, %.f, %.f \n", p.pos.x, p.pos.y, p.pos.z);
		if (m_pOctree->IsInNode(p.pos))
		{
			m_pOctree->AddObject(&p, idx);
		}
		idx += 1;
	}
	UpdateNodeTexture();
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
				if (curr->GetChildNode(i)->GetObjectArray().size() != 0)
					DrawCube(curr->GetChildNode(i)->GetPos(), curr->GetChildNode(i)->GetWidth());
				toProcess.push_back(curr->GetChildNode(i));
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
	
	//DrawParticle();
	//DrawSSBOsParticle();
	
	if(m_UpdateSwitch) UpdateSSBO();
	DrawSSBOParticle();

	// Octree
	UpdateOctree();
	//DrawOctreee();

	//printf("x : %.f, y : %.f , z : %.f\n", m_pCamera->GetPos().x, m_pCamera->GetPos().y, m_pCamera->GetPos().z);

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
