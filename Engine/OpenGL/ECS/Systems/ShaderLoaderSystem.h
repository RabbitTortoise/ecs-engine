#pragma once

#include <Engine/ECS/System.h>			// System declaration
#include <glm/glm.hpp>		// glm transform functions.
#include <glad/gl.h>					// Include glad

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>

class Shader;

class ShaderLoaderSystem : public IComponentTypeQuerySystem
{
public:


	ShaderLoaderSystem(EntityManager* entityManager);
	~ShaderLoaderSystem();
	
	//System class overrides
	void Init() override;
	void Destroy() override;
	void PreUpdate(float deltaTime) override;
	void ComponentTypeQueryUpdate(float deltaTime, IEntityStorage* entityTypeStorage) override;

	
	//Shader Loader System functionality
	
	int LoadShaderFromFile(std::string vertexShaderFilePath, std::string fragmentShaderFilePath);
	Shader* GetShader(int shaderID);

	std::vector<Shader*> Shaders;
	std::map<std::string, int> ShaderList;
	
	inline static const std::string SystemName = "ShaderLoaderSystem";
	inline static const System::SystemType SystemType = System::SystemType::System;
private:


};
