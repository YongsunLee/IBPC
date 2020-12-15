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

unsigned int g_ClientWidth = 500;
unsigned int g_ClientHeight = 500;

Renderer* g_Renderer = NULL;
Camera* g_MainCamera = NULL;

float g_RunRate = 0.0f;

void RenderScene(void)
{
	auto start = std::chrono::high_resolution_clock::now();

	// Renderer Test
	g_Renderer->DrawSystem();

	glutSwapBuffers();
	auto du = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();

	std::string title = "Octree Particle Collision System | ";
	std::string str_du = std::to_string(du);
	title += str_du.data();
	title += "ms";

	glutSetWindowTitle(title.data());
	//std::cout << du << "\n";
	//std::cout << du << std::endl;
}

void Idle(void)
{
	RenderScene();
}

void MouseInput(int button, int state, int x, int y)
{
	g_MainCamera->MouseInput(x, y, button, state);
	switch (button) {
	case 3: // wheel up
		g_RunRate += 0.02f;
		if (g_RunRate > 1.0f) g_RunRate = 1.0f;
		break;
	case 4: // wheel down
		g_RunRate -= 0.02f;
		if (g_RunRate < -0.75f) g_RunRate = -0.76f;
		break;
	}
	RenderScene();
}

void MouseMove(int x, int y)
{
	g_MainCamera->MouseInput(x, y);
	RenderScene();
}

void KeyInput(unsigned char key, int x, int y)
{
	g_MainCamera->KeyInput(key, x, y);
	switch (key) {
	case VK_ESCAPE: glutExit(); break;
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

int main(int argc, char **argv)
{
	// Initialize GL things
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(g_ClientWidth, g_ClientHeight);
	glutCreateWindow("GLSL KPU");

	glewInit();
	if (glewIsSupported("GL_VERSION_4_6"))
	{
		std::cout << " GL Version is 4.6\n ";
	}
	else
	{
		std::cout << "GLEW 4.6 not supported\n ";
	}

	// Initialize Renderer
	g_MainCamera = new Camera();
	g_Renderer = new Renderer(g_ClientWidth, g_ClientHeight);
	g_Renderer->SetCamera(g_MainCamera);

	glutDisplayFunc(RenderScene);
	glutIdleFunc(Idle);
	glutKeyboardFunc(KeyInput);
	glutSpecialFunc(SpecialKeyInput);
	glutMouseFunc(MouseInput);
	glutMotionFunc(MouseMove);
	glutMainLoop();

	delete g_Renderer;
	delete g_MainCamera;

    return 0;
}

