#include <RenderSystem.h>
#include <MatrixCalculations.h>

#include <iostream>

#include <Engine/OpenGL/ECS/Components/MeshComponent.h>
#include <Engine/OpenGL/ECS/Components/MaterialComponent.h>
#include <Engine/ECS/Components/TransformComponent.h>
#include <Engine/Interface/WindowManager.h>
#include <Engine/OpenGL/ECS/Systems/CameraControllerSystem.h>
#include <Engine/OpenGL/ECS/Entities/CameraEntity.h>

#include <Engine/OpenGL/ECS/Systems/TextureLoaderSystem.h>
#include <Engine/OpenGL/ECS/Systems/ShaderLoaderSystem.h>
#include <Engine/OpenGL/ECS/Systems/MeshLoaderSystem.h>



RenderSystem::RenderSystem(EntityManager* entityManager, CameraControllerSystem* cameraSystem, TextureLoaderSystem* textureSystem, ShaderLoaderSystem* shaderSystem, MeshLoaderSystem* meshLoader) : IComponentTypeQuerySystem(entityManager)
{
	EnvironmentalCubemap = 0;
	DefaultSkyboxTexture = 0;
	IrradianceMap = 0;
	PrefilterMap = 0;
	BRDFLUTTexture = 0;

	EquirectangularToCubemapShader = nullptr;
	BackgroundShader = nullptr;
	IrradianceShader = nullptr;
	PrefilterShader = nullptr;
	BRDFShader = nullptr;
	UnlitShader = nullptr;

	CameraSystem = cameraSystem;
	ShaderSystem = shaderSystem;
	TextureSystem = textureSystem;
	MeshLoader = meshLoader;

	TransformComponentIndex = WorldEntityManager->GetComponentTypeIdByName("TransformComponent");
	MaterialComponentIndex = WorldEntityManager->GetComponentTypeIdByName("MaterialComponent");
	MeshComponentIndex = WorldEntityManager->GetComponentTypeIdByName("MeshComponent");
	CameraComponentIndex = WorldEntityManager->GetComponentTypeIdByName("CameraComponent");
}

RenderSystem::~RenderSystem()
{

}



void RenderSystem::Init()
{
	WindowManager::RegisterToWindowSizeUpdate(&RenderSystem::WindowResized, this);


	// Enable depth buffering
	glDepthFunc(GL_LEQUAL);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	
	// Enable face culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK); //GL_BACK //GL_FRONT //GL_FRONT_AND_BACK
	glFrontFace(GL_CCW); //GL_CCW //GL_CW
	

	glDisable(GL_BLEND);
	

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	

	DefaultSkyboxTexture = TextureSystem->LoadHDRTextureFromFile("../Engine/Assets/kloppenheim_06_4k.hdr");
	EquirectangularToCubemapShader = ShaderSystem->GetShader(ShaderSystem->LoadShaderFromFile("../Engine/OpenGL/Shaders/cubemap.vert", "../Engine/OpenGL/Shaders/Equirectangular_to_cubemap.frag"));
	IrradianceShader = ShaderSystem->GetShader(ShaderSystem->LoadShaderFromFile("../Engine/OpenGL/Shaders/Cubemap.vert", "../Engine/OpenGL/Shaders/Irradiance_convolution.frag"));
	BackgroundShader = ShaderSystem->GetShader(ShaderSystem->LoadShaderFromFile("../Engine/OpenGL/Shaders/Background.vert", "../Engine/OpenGL/Shaders/Background.frag"));
	PrefilterShader = ShaderSystem->GetShader(ShaderSystem->LoadShaderFromFile("../Engine/OpenGL/Shaders/Cubemap.vert", "../Engine/OpenGL/Shaders/Prefilter.frag"));
	BRDFShader = ShaderSystem->GetShader(ShaderSystem->LoadShaderFromFile("../Engine/OpenGL/Shaders/BRDF.vert", "../Engine/OpenGL/Shaders/BRDF.frag"));

	UnlitShader = ShaderSystem->GetShader(ShaderSystem->LoadShaderFromFile("../Engine/OpenGL/Shaders/unlit.vert", "../Engine/OpenGL/Shaders/Unlit.frag"));


	SetupSkybox(DefaultSkyboxTexture);
}

void RenderSystem::Destroy()
{

}

void RenderSystem::PreUpdate(float deltaTime)
{
	// Query the size of the framebuffer (window content) from glfw.
	int width = WindowManager::GetWindowWidth();
	int height = WindowManager::GetWindowHeight();

	// Setup the opengl wiewport (i.e specify area to draw)
	glViewport(0, 0, width, height);
	
	// Set color to be used when clearing the screen
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	 
	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
}

void RenderSystem::ComponentTypeQueryUpdate(float deltaTime, IEntityStorage* entityTypeStorage)
{

	EntityTypeStorage<Entity>* entityStorage = static_cast<EntityTypeStorage<Entity>*>(entityTypeStorage);
	int entityType = entityStorage->GetEntityType();
	int entityCount = entityStorage->GetEntityCount();

	CameraEntity* RenderCamera = CameraSystem->GetMainCamera();
	TransformComponent* cameraTransformComponent = WorldEntityManager->GetComponent<TransformComponent>(RenderCamera);
	CameraComponent* cameraComponent = WorldEntityManager->GetComponent<CameraComponent>(RenderCamera);

	glm::mat4 viewMatrix = cameraComponent->Orientation;
	glm::mat4 projectionMatrix = cameraComponent->ProjectionMatrix;


	std::vector<InstanceProperties*> DifferentInstances;
	size_t previousInstance = -1;
	int meshVAO = -1;

	//Constructing instanced model matrix vectors
	for (int i = 0; i < entityStorage->GetEntityCount(); i++)
	{
		Entity* e = entityStorage->GetEntity(i);
		if (e != nullptr)
		{
			MeshComponent* meshComponent = WorldEntityManager->GetComponent<MeshComponent>(e);
			MaterialComponent* materialComponent = WorldEntityManager->GetComponent<MaterialComponent>(e);
			TransformComponent* transformComponent = WorldEntityManager->GetComponent<TransformComponent>(e);		

			if (materialComponent->HiddenInGame)
			{
				continue;
			}

			glm::mat4 modelMatrix;
			if (transformComponent->UseQuaternion)
			{
				modelMatrix = MatrixCalculations::CalculateModelMatrix(transformComponent->Position, transformComponent->Orientation, transformComponent->Scale);
			}
			else
			{
				modelMatrix = MatrixCalculations::CalculateModelMatrix(transformComponent->Position, transformComponent->EulerAngles, transformComponent->Scale);
			}

			if (previousInstance == -1)
			{
				InstanceProperties* newInstances = new InstanceProperties();
				newInstances->Mesh = meshComponent;
				newInstances->Material = materialComponent;
				newInstances->ModelMatrices = new std::vector<glm::mat4>();
				DifferentInstances.push_back(newInstances);
				DifferentInstances[0]->ModelMatrices->push_back(modelMatrix);
				previousInstance = 0;
				continue;
			}

			if (meshComponent->VAO == DifferentInstances[previousInstance]->Mesh->VAO &&
				AreMaterialComponentsEqual(DifferentInstances[previousInstance]->Material, materialComponent))
			{
				DifferentInstances[previousInstance]->ModelMatrices->push_back(modelMatrix);
			}
			else
			{
				size_t correctInstance = -1;
				for (size_t ins = 0; ins < DifferentInstances.size(); ins++)
				{
					if (ins == previousInstance) { continue; }
					if (meshComponent->VAO == DifferentInstances[previousInstance]->Mesh->VAO &&
						AreMaterialComponentsEqual(DifferentInstances[previousInstance]->Material, materialComponent))
					{
						correctInstance = ins;
						break;
					}
				}
				if (correctInstance == -1)
				{
					InstanceProperties* newInstances = new InstanceProperties();
					newInstances->Mesh = meshComponent;
					newInstances->Material = materialComponent;
					newInstances->ModelMatrices = new std::vector<glm::mat4>();
					DifferentInstances.push_back(newInstances);
					DifferentInstances[DifferentInstances.size() - 1]->ModelMatrices->push_back(modelMatrix);
					previousInstance = DifferentInstances.size() - 1;
					continue;
				}
				else
				{
					DifferentInstances[correctInstance]->ModelMatrices->push_back(modelMatrix);
					previousInstance = correctInstance;
				}
			}
		}
	}


	//Rendering instanced models
	for (size_t i = 0; i < DifferentInstances.size(); i++)
	{
		MeshLoader->MakeInstanced(DifferentInstances[i]->Mesh, static_cast<int>(DifferentInstances[i]->ModelMatrices->size()), &DifferentInstances[i]->ModelMatrices->at(0));

		//Bind Mesh VAO
		glBindVertexArray(DifferentInstances[i]->Mesh->VAO);

		//Set Shader Properties
		Shader* RenderShader = ShaderSystem->GetShader(DifferentInstances[i]->Material->ShaderID);
		MaterialComponent* materialComponent = DifferentInstances[i]->Material;

		RenderShader->Bind();
		RenderShader->SetMat4("in_Projection", projectionMatrix);
		RenderShader->SetMat4("in_View", viewMatrix);

		//RenderShader->SetVec4("in_camPos", cameraPosition);
		RenderShader->SetVec4("camPos", glm::vec4(cameraTransformComponent->Position, 1.0f));

		RenderShader->SetVec3("lightPositions[0]", DirectionalLight);
		RenderShader->SetVec3("lightColors[0]", DirectionalLightColor);
		RenderShader->SetVec3("lightPositions[1]", LightPosition);
		RenderShader->SetVec3("lightColors[1]", LightColor);

		RenderShader->SetFloat("textureRepeat", materialComponent->TextureRepeat);
		RenderShader->SetFloat("parallaxScale", materialComponent->ParallaxScale);
		RenderShader->SetInt("IBLEnabled", materialComponent->IBLEnabled);

		if (materialComponent->Texture_Albedo > 0) {
			RenderShader->SetInt("albedoMap", 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, materialComponent->Texture_Albedo);
		}
		if (materialComponent->Texture_Normal > 0) {
			RenderShader->SetInt("normalMap", 1);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, materialComponent->Texture_Normal);
		}
		if (materialComponent->Texture_Depth > 0) {
			RenderShader->SetInt("depthMap", 2);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, materialComponent->Texture_Depth);
		}
		if (materialComponent->Texture_Metallic > 0) {
			RenderShader->SetInt("metallicMap", 3);
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, materialComponent->Texture_Metallic);
		}
		if (materialComponent->Texture_Roughness > 0) {
			RenderShader->SetInt("roughnessMap", 4);
			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, materialComponent->Texture_Roughness);
		}
		if (materialComponent->Texture_AO > 0) {
			RenderShader->SetInt("aoMap", 5);
			glActiveTexture(GL_TEXTURE5);
			glBindTexture(GL_TEXTURE_2D, materialComponent->Texture_AO);
		}
		if (IrradianceMap > 0) {
			RenderShader->SetInt("irradianceMap", 6);
			glActiveTexture(GL_TEXTURE6);
			glBindTexture(GL_TEXTURE_CUBE_MAP, IrradianceMap);
		}
		if (PrefilterMap > 0) {
			RenderShader->SetInt("prefilterMap", 7);
			glActiveTexture(GL_TEXTURE7);
			glBindTexture(GL_TEXTURE_CUBE_MAP, PrefilterMap);
		}
		if (BRDFLUTTexture > 0) {
			RenderShader->SetInt("brdfLUT", 8);
			glActiveTexture(GL_TEXTURE8);
			glBindTexture(GL_TEXTURE_2D, BRDFLUTTexture);
		}

		GLsizei amount = static_cast<GLsizei>(DifferentInstances[i]->ModelMatrices->size());
		glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(DifferentInstances[i]->Mesh->IndiceCount), GL_UNSIGNED_INT, 0, amount);
		glFinish();
		glBindVertexArray(0);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//Memory Cleanup
	for (size_t i = 0; i < DifferentInstances.size(); i++)
	{
		if (DifferentInstances[i]->ModelMatrices != nullptr)
		{
			delete DifferentInstances[i]->ModelMatrices;
			DifferentInstances[i]->ModelMatrices = nullptr;
		}
		delete DifferentInstances[i];
		DifferentInstances[i] = nullptr;
	}
}

void RenderSystem::PostUpdate(float deltaTime)
{

	CameraEntity* RenderCamera = CameraSystem->GetMainCamera();
	TransformComponent* cameraTransformComponent = WorldEntityManager->GetComponent<TransformComponent>(RenderCamera);
	CameraComponent* cameraComponent = WorldEntityManager->GetComponent<CameraComponent>(RenderCamera);

	glm::mat4 viewMatrix = cameraComponent->Orientation;

	glm::mat4 projectionMatrix = cameraComponent->ProjectionMatrix;

	/*
	// Debug BRDF Render
	glm::mat4 model = MatrixCalculations::CalculateModelMatrix(glm::vec3(3, 0, 0), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
	UnlitShader->Bind();
	UnlitShader->SetMat4("in_model", model);
	UnlitShader->SetMat4("in_view", viewMatrix);
	UnlitShader->SetMat4("in_projection", projectionMatrix);
	if (EnvironmentalCubemap) {
		UnlitShader->SetInt("textureMap", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, BRDFLUTTexture);
	}
	RenderQuad();
	*/

	glFrontFace(GL_CW); //GL_CCW //GL_CW
	// render skybox (render as last to prevent overdraw)
	BackgroundShader->Bind();
	BackgroundShader->SetMat4("in_View", viewMatrix);
	BackgroundShader->SetMat4("in_Projection", projectionMatrix);
	if (EnvironmentalCubemap) {
		BackgroundShader->SetInt("environmentMap", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, EnvironmentalCubemap);
		//glBindTexture(GL_TEXTURE_CUBE_MAP, IrradianceMap);
		//glBindTexture(GL_TEXTURE_CUBE_MAP, PrefilterMap);
		
	}
	RenderCube();
	glFinish();
	glFrontFace(GL_CCW); //GL_CCW //GL_CW

	
}

void RenderSystem::SetupSkybox(int hdrTextureHandle)
{

	glFrontFace(GL_CW); //GL_CCW //GL_CW
	// pbr: setup framebuffer
	// ----------------------
	unsigned int captureFBO;
	unsigned int captureRBO;
	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 2048, 2048);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

	// pbr: load the HDR environment map
   // ---------------------------------

	glGenTextures(1, &EnvironmentalCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, EnvironmentalCubemap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 2048, 2048, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	// pbr: set up projection and view matrices for capturing data onto the 6 cubemap face directions
	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureViews[] =
	{
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};


	// pbr: convert HDR equirectangular environment map to cubemap equivalent
	// ----------------------------------------------------------------------
	EquirectangularToCubemapShader->Bind();
	EquirectangularToCubemapShader->SetInt("equirectangularMap", 0);
	EquirectangularToCubemapShader->SetMat4("projection", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, hdrTextureHandle);

	glViewport(0, 0, 2048, 2048); // don't forget to configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	for (unsigned int i = 0; i < 6; ++i)
	{
		EquirectangularToCubemapShader->SetMat4("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, EnvironmentalCubemap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		RenderCube();
		glFinish();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// pbr: create an irradiance cubemap, and re-scale capture FBO to irradiance scale.
	// --------------------------------------------------------------------------------
	glGenTextures(1, &IrradianceMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, IrradianceMap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 124, 124, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 124, 124);

	// pbr: solve diffuse integral by convolution to create an irradiance (cube)map.
	// -----------------------------------------------------------------------------
	IrradianceShader->Bind();
	IrradianceShader->SetInt("environmentMap", 0);
	IrradianceShader->SetMat4("projection", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, EnvironmentalCubemap);

	glViewport(0, 0, 124, 124); // don't forget to configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	for (unsigned int i = 0; i < 6; ++i)
	{
		IrradianceShader->SetMat4("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, IrradianceMap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		RenderCube();
		glFinish();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);



	// pbr: create a pre-filter cubemap, and re-scale capture FBO to pre-filter scale.
	// --------------------------------------------------------------------------------
	glGenTextures(1, &PrefilterMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, PrefilterMap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // be sure to set minification filter to mip_linear 
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// generate mipmaps for the cubemap so OpenGL automatically allocates the required memory.
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	// pbr: run a quasi monte-carlo simulation on the environment lighting to create a prefilter (cube)map.
	// ----------------------------------------------------------------------------------------------------
	PrefilterShader->Bind();
	PrefilterShader->SetInt("environmentMap", 0);
	PrefilterShader->SetMat4("projection", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, EnvironmentalCubemap);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	unsigned int maxMipLevels = 5;
	for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
	{
		// reisze framebuffer according to mip-level size.
		unsigned int mipWidth = static_cast<unsigned int>(512 * std::pow(0.5, mip));
		unsigned int mipHeight = static_cast<unsigned int>(512 * std::pow(0.5, mip));
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
		glViewport(0, 0, mipWidth, mipHeight);

		float roughness = (float)mip / (float)(maxMipLevels - 1);
		PrefilterShader->SetFloat("roughness", roughness);
		for (unsigned int i = 0; i < 6; ++i)
		{
			PrefilterShader->SetMat4("view", captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, PrefilterMap, mip);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			RenderCube();
			glFinish();
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	glFrontFace(GL_CCW); //GL_CCW //GL_CW

	// pbr: generate a 2D LUT from the BRDF equations used.
	// ----------------------------------------------------
	glGenTextures(1, &BRDFLUTTexture);

	// pre-allocate enough memory for the LUT texture.
	glBindTexture(GL_TEXTURE_2D, BRDFLUTTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 2048, 2048, 0, GL_RG, GL_FLOAT, 0);
	// be sure to set wrapping mode to GL_CLAMP_TO_EDGE
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// then re-configure capture framebuffer object and render screen-space quad with BRDF shader.
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 2048, 2048);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, BRDFLUTTexture, 0);

	glViewport(0, 0, 2048, 2048);
	BRDFShader->Bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	RenderQuad();
	glFinish();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}


void RenderSystem::WindowResized(int width, int height)
{
	CameraSystem->UpdateCameraProjectionMatrix(CameraSystem->GetMainCamera(), width, height, false);
}


// renderCube() renders a 1x1 3D cube in NDC.
// -------------------------------------------------
void RenderSystem::RenderCube()
{
	MeshComponent meshComponent = MeshLoader->GetPrimitiveShape(MeshLoaderSystem::PrimitiveShape::Cube);
	// render Cube
	glBindVertexArray(meshComponent.VAO);
	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(meshComponent.IndiceCount), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------

void RenderSystem::RenderQuad()
{
	MeshComponent meshComponent = MeshLoader->GetPrimitiveShape(MeshLoaderSystem::PrimitiveShape::Quad);
	// render Cube
	glBindVertexArray(meshComponent.VAO);
	glDrawElements(GL_TRIANGLE_STRIP, static_cast<GLsizei>(meshComponent.IndiceCount), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

bool RenderSystem::AreMaterialComponentsEqual(MaterialComponent* c1, MaterialComponent* c2)
{
	if (c1->ShaderID != c2->ShaderID) { return false; }
	if (c1->Texture_Albedo != c1->Texture_Albedo) { return false; }
	if (c1->Texture_Depth != c2->Texture_Depth) { return false; }
	if (c1->Texture_Normal != c2->Texture_Normal) { return false; }
	if (c1->Texture_Metallic != c2->Texture_Metallic) { return false; }
	if (c1->Texture_Roughness != c2->Texture_Roughness) { return false; }
	if (c1->Texture_AO != c2->Texture_AO) { return false; }
	if (c1->TextureRepeat != c2->TextureRepeat) { return false; }
	if (c1->ParallaxScale != c2->ParallaxScale) { return false; }
	if (c1->IBLEnabled != c2->IBLEnabled) { return false; }

	return true;
}


/*
ShaderID;
Texture_Albedo;
Texture_Depth;
Texture_Normal;
Texture_Metallic;
Texture_Roughness;
Texture_AO;

TextureRepeat;
ParallaxScale;
IBLEnabled;
*/