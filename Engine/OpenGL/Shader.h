#pragma once                // Include guard. For more information, see: https://en.wikipedia.org/wiki/Pragma_once

#include <string>
#include <glm/glm.hpp>			// Include glm
#include <glad/gl.h>		// Include glad
#include <GLFW/glfw3.h>		// Include glfw

class Shader{
public:
	Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);
	~Shader();
	void Bind();

	//void SetUniform(const std::string& name, float x, float y, float z);
	//void SetUniform(const std::string& name, const glm::mat4& m);
	//void SetUniform(const std::string& name, float x, float y, float z, float w);
	//void SetUniform(const std::string& name, int value);

	void SetBool(const std::string& name, bool value) const;
	void SetInt(const std::string& name, int value) const;
	void SetFloat(const std::string& name, float value) const;
	void SetVec2(const std::string& name, const glm::vec2& value) const;
	void SetVec2(const std::string& name, float x, float y) const;
	void SetVec3(const std::string& name, const glm::vec3& value) const;
	void SetVec3(const std::string& name, float x, float y, float z) const;
	void SetVec4(const std::string& name, const glm::vec4& value) const;
	void SetVec4(const std::string& name, float x, float y, float z, float w);
	void SetMat2(const std::string& name, const glm::mat2& mat) const;
	void SetMat3(const std::string& name, const glm::mat3& mat) const;
	void SetMat4(const std::string& name, const glm::mat4& mat) const;
	void SetMat4(GLuint location, const glm::mat4& mat) const;

	void SetModelUniformLocation(std::string name) { ModelUniformLocation = glGetUniformLocation(ShaderProgram, name.c_str()); }
	GLuint GetModelUniformLocation() { return ModelUniformLocation; }
	GLint GetShaderProgram() {return ShaderProgram;}

	void CheckCompileErrors(GLuint shader, std::string type);

private:
	// Handle to the shader program
	GLint ShaderProgram;
	GLuint ModelUniformLocation;
};
