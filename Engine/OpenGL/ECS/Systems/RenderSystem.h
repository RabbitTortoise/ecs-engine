#pragma once

#include <Engine/ECS/System.h>			// System declaration
#include <glm/glm.hpp>		// glm transform functions.
#include <glad/gl.h>				// Include glad


class Camera;
class Shader;

class CameraControllerSystem;
class TextureLoaderSystem;
class ShaderLoaderSystem;
class MeshLoaderSystem;

class MaterialComponent;
class RenderComponent;
class MeshComponent;

class RenderSystem : public IComponentTypeQuerySystem
{
	friend class InstancedRenderingSystem;

public:
	RenderSystem(EntityManager* entityManager, CameraControllerSystem* cameraSystem, TextureLoaderSystem* textureSystem, ShaderLoaderSystem* shaderSystem, MeshLoaderSystem* meshLoader);
	~RenderSystem();
	
	//System class overrides
	void Init() override;
	void Destroy() override;
	void PreUpdate(float deltaTime) override;
	void PostUpdate(float deltaTime) override;
	void ComponentTypeQueryUpdate(float deltaTime, IEntityStorage* entityTypeStorage) override;

	void SetupSkybox(int skyboxTextureHandle);


	inline static const std::string SystemName = "RenderSystem";
	inline static const System::SystemType SystemType = System::SystemType::RenderSystem;

private:

	//Frequently accessed systems
	CameraControllerSystem* CameraSystem;
	ShaderLoaderSystem* ShaderSystem;
	TextureLoaderSystem* TextureSystem;
	MeshLoaderSystem* MeshLoader;

	GLuint DefaultSkyboxTexture;
	GLuint EnvironmentalCubemap;
	GLuint IrradianceMap;
	GLuint PrefilterMap;
	GLuint BRDFLUTTexture;
	
	Shader* EquirectangularToCubemapShader;
	Shader* IrradianceShader;
	Shader* BackgroundShader;
	Shader* PrefilterShader;
	Shader* BRDFShader;
	Shader* UnlitShader;

	//PBR Shader testing
	glm::vec3 DirectionalLight = { 1.0f, -1.0, 0 };
	glm::vec3 DirectionalLightColor = { 0.01f, 0.01f, 0.01f };
	//glm::vec3 DirectionalLightColor = { 0.0f, 0.0f, 0.0f };

	glm::vec3 LightPosition = {0,2,0};
	glm::vec3 LightColor = {1.0f,0.1f,0.0f};
	//glm::vec3 LightColor = { 0.0f,0.0f,0.0f };

	//Window Resize event callback
	void WindowResized(int width, int height);

	void RenderCube();
	void RenderQuad();

	//Helper function, needs to be moved to a custom random class.
	float RandomNumber0_1()
	{
		float num = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX));
		return num;
	}

	int TransformComponentIndex = -1;
	int MaterialComponentIndex = -1;
	int MeshComponentIndex = -1;
	int CameraComponentIndex = -1;

	bool AreMaterialComponentsEqual(MaterialComponent* c1, MaterialComponent* c2);
	

	struct InstanceProperties
	{
		MeshComponent* Mesh = nullptr;
		MaterialComponent* Material = nullptr;
		std::vector<glm::mat4>* ModelMatrices = nullptr;
	};
};


/*

bool AddEntityTypeIfCompatible(int entityType, std::vector<std::string> componentTypes) override
	{
		if (IComponentTypeQuerySystem::AddEntityTypeIfCompatible(entityType, componentTypes))
		{
			//If system already exists, return
			auto it = CurrentEntityMaterialInUse.find(entityType);
			if (it != CurrentEntityMaterialInUse.end())
			{
				return true;
			}

			CurrentEntityMaterialInUse.insert(std::make_pair(entityType, 0));
			MaterialChangedOnEntity.insert(std::make_pair(entityType, std::vector<int>()));
			EntityTypesToCheck.push_back(entityType);
			return true;
		}
		return false;
	}

*/