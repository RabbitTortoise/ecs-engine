#pragma once

#include <Engine/ECS/Component.h>
#include <Engine/OpenGL/Shader.h>
#include <glad/gl.h>				// Include glad

class MeshComponent : public Component
{
public:
	MeshComponent() : Component()
	{ 
		VAO = 0;
		VBO = 0;
		EBO = 0;
		IndiceCount = 0;

	}
	MeshComponent(const MeshComponent& c) :

		VAO(c.VAO),
		VBO(c.VBO),
		EBO(c.EBO),
		IndiceCount(c.IndiceCount),

		Component(static_cast<Component>(c)) 
	{}
	MeshComponent(MeshComponent&&c) noexcept :

		VAO(std::move(c.VAO)),
		VBO(std::move(c.VBO)),
		EBO(std::move(c.EBO)),
		IndiceCount(std::move(c.IndiceCount)),
		Component(std::move(c)) 
	{}

	MeshComponent& operator=(const MeshComponent& c)
	{  
		VAO = c.VAO;
		VBO = c.VBO;
		EBO = c.EBO;
		IndiceCount = c.IndiceCount;
		return *this; 
	}

	GLuint VAO;					// Handle to Vertex array object
	GLuint VBO;					// Handle to Vertex buffer object
	GLuint EBO;					// Handle to Element buffer object
	size_t IndiceCount;

	inline static const std::string ComponentName = "MeshComponent";
	inline static int ComponentIndex = -1;
};
