/*
Copyright 2018 Lee Taek Hee (Korea Polytech University)

This program is free software: you can redistribute it and/or modify
it under the terms of the What The Hell License. Do it plz.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY.
*/

#include "stdafx.h"
#include "Renderer.h"
#include "Camera.h"

unsigned int g_ClientWidth = 1000;
unsigned int g_ClientHeight = 500;

Renderer* g_Renderer = nullptr;
Camera* g_RenderCamera = nullptr;
Camera* g_FixedCollisionDetectionCamera = nullptr;
Camera* g_CollisionDetectionCamera = nullptr;

std::string g_title = "Image-based Particle Collision System";

void RenderScene(void)
{
	auto start = std::chrono::high_resolution_clock::now();
	// Render
	g_Renderer->DrawParticleSystem();

	glutSwapBuffers();
	auto du = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();

	std::string title = "Image-based Particle Collision System | ";
	std::string str_du = std::to_string(du);
	title += str_du.data();
	title += "ms";

	glutSetWindowTitle(title.data());
	//std::cout << du << "\n";
}

void Idle(void)
{
	RenderScene();
}

void KeyInput(unsigned char key, int x, int y)
{
	g_RenderCamera->KeyInput(key, x, y);
	g_CollisionDetectionCamera->KeyInput(key, x, y);
	switch (key) {
	case VK_ESCAPE: 
		glutExit(); 
		break;
	case VK_TAB: 
		if (g_Renderer->m_UpdateSwitch == false) {
			g_Renderer->m_UpdateSwitch = true;
		}
  	    else 
			g_Renderer->m_UpdateSwitch = false; 
		break;
	}
	RenderScene();
}

void SpecialKeyInput(int key, int x, int y)
{
	RenderScene();
}

void MouseInput(int button, int state, int x, int y)
{
	g_RenderCamera->MouseInput(x, y, button, state);
	g_CollisionDetectionCamera->GetRotationSetting(g_RenderCamera);
	//g_CollisionDetectionCamera->MouseInput(x, y, button, state);
	RenderScene();
}

void MouseMove(int x, int y)
{
	g_RenderCamera->MouseInput(x, y);
	g_CollisionDetectionCamera->GetRotationSetting(g_RenderCamera);
	//g_CollisionDetectionCamera->MouseInput(x, y);
	RenderScene();
}

int main(int argc, char **argv)
{
	// Init GL things
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(g_ClientWidth, g_ClientHeight);
	glutCreateWindow("Image-based Particle Collision System");

	glewInit();
	if (GL_TRUE == glewIsSupported("GL_VERSION_4_6")) {
		std::cout << " GL Version is 4.6\n ";
	}
	else {
		std::cout << "GLEW 4.6 not supported\n ";
	}

	// Init
	g_RenderCamera = new Camera();
	g_FixedCollisionDetectionCamera = new Camera();
	g_CollisionDetectionCamera = new Camera();
	// CD Camera Offset 
	g_CollisionDetectionCamera->SetPos(glm::vec3(g_RenderCamera->GetPos().x, g_RenderCamera->GetPos().y, g_RenderCamera->GetPos().z));

	g_Renderer = new Renderer(g_ClientWidth, g_ClientHeight);
	g_Renderer->SetCamera(g_RenderCamera);
	g_Renderer->SetFixedCDCamera(g_FixedCollisionDetectionCamera);
	g_Renderer->SetCDCamera(g_CollisionDetectionCamera);

	glutDisplayFunc(RenderScene);
	glutIdleFunc(Idle);
	glutKeyboardFunc(KeyInput);
	glutSpecialFunc(SpecialKeyInput);
	glutMouseFunc(MouseInput);
	glutMotionFunc(MouseMove);
	glutMainLoop();

	delete g_Renderer;
	delete g_RenderCamera;
	delete g_FixedCollisionDetectionCamera;
	delete g_CollisionDetectionCamera;

    return 0;
}

