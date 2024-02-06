#include "IECS.h"

#include <vector>

#include <Engine/OpenGL/ECS/Components/MaterialComponent.h>
#include <Engine/OpenGL/ECS/Components/MeshComponent.h>
#include <Engine/ECS/Components/TransformComponent.h>
#include <Engine/OpenGL/ECS/Components/CameraComponent.h>
#include <Engine/ECS/Components/MovementComponent.h>
#include <Engine/ECS/Components/BoxColliderComponent.h>
#include <Engine/ECS/Components/RigidbodyComponent.h>

#include <Engine/OpenGL/ECS/Entities/CameraEntity.h>
#include <Engine/ECS/Entities/RigidbodyEntity.h>

#include <Engine/OpenGL/ECS/Systems/RenderSystem.h>
#include <Engine/OpenGL/ECS/Systems/CameraControllerSystem.h>
#include <Engine/OpenGL/ECS/Systems/MeshLoaderSystem.h>
#include <Engine/OpenGL/ECS/Systems/TextureLoaderSystem.h>
#include <Engine/OpenGL/ECS/Systems/ShaderLoaderSystem.h>
#include <Engine/ECS/Systems/PhysicsSystem.h>


#include <Engine/Interface/WindowManager.h>

namespace IECS
{
	namespace
	{
		EntityManager* WorldEntityManager;
		SystemsManager* WorldSystemsManager;
		RenderSystem* WorldRenderSystem;
		CameraControllerSystem* WorldCameraControllerSystem;
		MeshLoaderSystem* WorldMeshLoaderSystem;
		TextureLoaderSystem* WorldTextureLoaderSystem;
		ShaderLoaderSystem* WorldShaderLoaderSystem;
		PhysicsSystem* WorldPhysicsSystem;
	}

	void Init()
	{
		WorldEntityManager = new EntityManager();
		WorldSystemsManager = new SystemsManager(WorldEntityManager);

		//Create storages to engine components
		WorldEntityManager->CreateComponentType<TransformComponent>();
		WorldEntityManager->CreateComponentType<MaterialComponent>();
		WorldEntityManager->CreateComponentType<MeshComponent>();
		WorldEntityManager->CreateComponentType<CameraComponent>();
		WorldEntityManager->CreateComponentType<MovementComponent>();
		WorldEntityManager->CreateComponentType<BoxColliderComponent>();
		WorldEntityManager->CreateComponentType<RigidbodyComponent>();

		//Create engine WorldCameraControllerSystem
		WorldEntityManager->CreateEntityTypeStorage<CameraEntity>();
		//CameraEntity::AssingComponentIDs();
		WorldCameraControllerSystem = new CameraControllerSystem(WorldEntityManager, WorldEntityManager->GetEntityTypeId<CameraEntity>());
		WorldSystemsManager->AddSystem(WorldCameraControllerSystem);


		WorldCameraControllerSystem->AddRequiredComponentType<TransformComponent>();
		WorldCameraControllerSystem->AddRequiredComponentType<CameraComponent>();
		WorldCameraControllerSystem->AddRequiredComponentType<MovementComponent>();
		WorldCameraControllerSystem->AddEntityTypeIfCompatible<CameraEntity>();

		//Create new camera and set it as main camera
		WorldCameraControllerSystem->SetMainCamera(WorldCameraControllerSystem->CreateCamera(WindowManager::GetWindowWidth(), WindowManager::GetWindowHeight(), false, 90.0f, 0.1f, 100.0f));

		//Create engine WorldMeshLoaderSystem
		WorldMeshLoaderSystem= new MeshLoaderSystem(WorldEntityManager);
		WorldSystemsManager->AddSystem(WorldMeshLoaderSystem);
		
		//Create engine ShaderLoaderSystem
		WorldShaderLoaderSystem = new ShaderLoaderSystem(WorldEntityManager);
		WorldSystemsManager->AddSystem(WorldShaderLoaderSystem);

		//Create engine TextureLoaderSystem
		WorldTextureLoaderSystem = new TextureLoaderSystem(WorldEntityManager);
		WorldSystemsManager->AddSystem(WorldTextureLoaderSystem);

		//Create engine WorldRenderSystem
		WorldRenderSystem = new RenderSystem(WorldEntityManager, WorldCameraControllerSystem, WorldTextureLoaderSystem, WorldShaderLoaderSystem, WorldMeshLoaderSystem);
		WorldSystemsManager->AddSystem(WorldRenderSystem);
		WorldRenderSystem->AddRequiredComponentType<TransformComponent>();
		WorldRenderSystem->AddRequiredComponentType<MeshComponent>();
		WorldRenderSystem->AddRequiredComponentType<MaterialComponent>();


		

		WorldPhysicsSystem = new PhysicsSystem(WorldEntityManager);
		WorldSystemsManager->AddSystem(WorldPhysicsSystem);
		WorldPhysicsSystem->AddRequiredComponentType<TransformComponent>();
		WorldPhysicsSystem->AddRequiredComponentType<BoxColliderComponent>();
		WorldPhysicsSystem->AddRequiredComponentType<RigidbodyComponent>();

		//Create engine PhysicsSystem
		WorldEntityManager->CreateEntityTypeStorage<RigidbodyEntity>();
		WorldPhysicsSystem->AddEntityTypeIfCompatible<RigidbodyEntity>();
		WorldRenderSystem->AddEntityTypeIfCompatible<RigidbodyEntity>();



	}

	void Destroy()
	{
		WorldSystemsManager->DestroySystems();
		WorldEntityManager->ResetAllStorages();
		delete WorldEntityManager;
		delete WorldSystemsManager;
	}

	void ResetECSWorld()
	{
		WorldSystemsManager->DestroySystems();
		WorldEntityManager->ResetAllStorages();
		WorldSystemsManager->InitSystems();
	}

	EntityManager* GetEntityManager()
	{
		return WorldEntityManager;
	}

	SystemsManager* GetSystemsManager()
	{
		return WorldSystemsManager;
	}
}