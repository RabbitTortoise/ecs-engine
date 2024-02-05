#include <ShaderLoaderSystem.h>
#include <iostream>
#include <shader.h>


ShaderLoaderSystem::ShaderLoaderSystem(EntityManager* entityManager) : IComponentTypeQuerySystem(entityManager)
{
	
}

ShaderLoaderSystem::~ShaderLoaderSystem()
{
	Destroy();
}

void ShaderLoaderSystem::Init()
{
	
}

void ShaderLoaderSystem::Destroy()
{
	for (auto s : Shaders)
	{
		if (s != nullptr)
		{
			delete s;
			s = nullptr;
		}
	}
}

void ShaderLoaderSystem::PreUpdate(float deltaTime)
{
	
}

void ShaderLoaderSystem::ComponentTypeQueryUpdate(float deltaTime, IEntityStorage* entityTypeStorage)
{


}


int ShaderLoaderSystem::LoadShaderFromFile(std::string vertexShaderFilePath, std::string fragmentShaderFilePath)
{
	std::string filePathsCombined = vertexShaderFilePath + fragmentShaderFilePath;

	auto it = ShaderList.find(filePathsCombined);
	if (it == ShaderList.end())
	{
		Shaders.emplace_back(new Shader(vertexShaderFilePath.c_str(), fragmentShaderFilePath.c_str()));
		int index = static_cast<int>(Shaders.size() - 1);
		ShaderList.insert(std::pair(filePathsCombined, index));
		return index;
	}
	else
	{
		return it->second;
	}
}

Shader* ShaderLoaderSystem::GetShader(int shaderID)
{
	if (shaderID < Shaders.size())
	{
		return Shaders[shaderID];
	}
	return nullptr;
}