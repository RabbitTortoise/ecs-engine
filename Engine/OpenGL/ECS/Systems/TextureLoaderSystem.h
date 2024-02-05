#pragma once

#include <Engine/ECS/System.h>			// System declaration
#include <glm/glm.hpp>		// glm transform functions.
#include <glad/gl.h>					// Include glad

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>

class Texture;

class TextureLoaderSystem : public IComponentTypeQuerySystem
{
public:


	TextureLoaderSystem(EntityManager* entityManager);
	~TextureLoaderSystem();
	
	//System class overrides
	void Init() override;
	void Destroy() override;
	void PreUpdate(float deltaTime) override;
	void ComponentTypeQueryUpdate(float deltaTime, IEntityStorage* entityTypeStorage) override;

	
	//Texture Loader System functionality
	
	int LoadTextureFromFile(std::string filePath);
	int LoadHDRTextureFromFile(std::string filePath);
	
	//Shader and Texture Storage
	
	std::vector<int> TextureHandles;
	std::map<std::string, int> TextureList;
	
	std::vector<int> HDRTextureHandles;
	std::map<std::string, int> HDRTextureList;

	inline static const std::string SystemName = "TextureLoaderSystem";
	inline static const System::SystemType SystemType = System::SystemType::System;
private:
	int CreateTexture(int width, int height, int nrChannels, GLubyte* data);
	int CreateHDRTexture(int width, int height, int nrChannels, float* data);

};
