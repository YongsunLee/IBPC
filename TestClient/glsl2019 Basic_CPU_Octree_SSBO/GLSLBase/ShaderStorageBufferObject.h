#pragma once

class ShaderStorageBufferObject {
public:
	ShaderStorageBufferObject(size_t size, GLenum dataMode = GL_DYNAMIC_COPY);
	
	template <typename T> 
	ShaderStorageBufferObject(GLenum dataMode, std::vector<T>& data) {
		glGenBuffers(1, &m_SSBO);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBO);
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(T) * data.size(), &data[0], dataMode);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}
	
	virtual ~ShaderStorageBufferObject();
	
	void bind();
	void bindBase(GLuint idx);

	template <typename T>
	void setData(const std::vector<T>& data) {
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBO);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(T) * data.size(), &data[0]);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

private:
	GLuint			m_SSBO;
};
