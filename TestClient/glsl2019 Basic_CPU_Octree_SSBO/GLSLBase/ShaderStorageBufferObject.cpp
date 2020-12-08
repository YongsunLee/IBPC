#include "stdafx.h"
#include "ShaderStorageBufferObject.h"

ShaderStorageBufferObject::ShaderStorageBufferObject(size_t size, GLenum dataMode)
{
	glGenBuffers(1, &m_SSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, size, NULL, dataMode);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

ShaderStorageBufferObject::~ShaderStorageBufferObject()
{
	glDeleteBuffers(1, &m_SSBO);
}

void ShaderStorageBufferObject::bind()
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBO);
}

void ShaderStorageBufferObject::bindBase(GLuint idx)
{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, idx, m_SSBO);
}