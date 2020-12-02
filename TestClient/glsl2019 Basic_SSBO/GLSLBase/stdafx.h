#pragma once

#include "targetver.h"

// Window 헤더
#include <Windows.h>

// C++ 런타임 라이브러리
#include <iostream>
#include <fstream>
#include <cassert>
#include <cfloat>
#include <cstdint>
#include <cmath>
#include <random>
#include <algorithm>
#include <string>
#include <unordered_map>
#include <vector>
#include <string>
#include <chrono>
#include <list>

// OpenGL 라이브러리
#include "Dependencies\glew.h"
#include "Dependencies\wglew.h"

// Load Png
#include "LoadPng.h"

// GLMath 라이브러리
#include "Dependencies\glm/glm.hpp"
#include "Dependencies\glm/gtc/matrix_transform.hpp"
#include "Dependencies\glm/gtx/euler_angles.hpp"
#include "Dependencies\glm/gtx/quaternion.hpp"

// freeglut 라이브러리
#include "Dependencies\freeglut.h"

// assimp for import Model
#include "Dependencies/assimp/Importer.hpp"
#include "Dependencies/assimp/cimport.h"
#include "Dependencies/assimp/postprocess.h"
#include "Dependencies/assimp/scene.h"

#pragma comment(lib, "assimp.lib")

extern unsigned int g_ClientWidth;
extern unsigned int g_ClientHeight;

constexpr ptrdiff_t LOG_SIZE = 1024;

constexpr float FLT_PI = 3.141592f;
constexpr float DEGREE_TO_RADIAN(float degree) { return (FLT_PI * degree) / 180.0f; }

int RAND_INT();
int RAND_INT(int min, int max);
float RAND_FLOAT();
float RAND_FLOAT(float min, float max);

unsigned char* loadBMPRaw(const char* imagepath, unsigned int& outWidth, unsigned int& outHeight);
GLuint CreatePngTexture(char* filePath);
GLuint CreateBmpTexture(char* filePath);
