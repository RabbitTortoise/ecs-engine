#pragma once

#include <Engine/ECS/System.h>
#include <Engine/ECS/Components/TransformComponent.h>
#include <Game/Components/TriggerComponent.h>
#include <Engine/OpenGL/ECS/Systems/CameraControllerSystem.h>
#include <Engine/OpenGL/ECS/Entities/CameraEntity.h>
#include <iostream>

class TriggerSystem : public IComponentTypeQuerySystem
{
public:
	TriggerSystem(EntityManager* entityManager, CameraControllerSystem* cameraSystem, CollectingGame* gameScript) : IComponentTypeQuerySystem(entityManager)
	{
		CameraSystem = cameraSystem;
		GameScript = gameScript;
	}

	virtual void Init()
	{
		
	}

	virtual void Destroy()
	{
	}

	virtual void PreUpdate(float deltaTime)
	{

	}

	void ComponentTypeQueryUpdate(float deltaTime, IEntityStorage* entityTypeStorage)
	{
		
		EntityTypeStorage<Entity>* entityStorage = static_cast<EntityTypeStorage<Entity>*>(entityTypeStorage);
		//std::cout << "EntityMovementSystem: Entity count: " << std::to_string(entityStorage->GetEntityCount()) << std::endl;

		for (int i = 0; i < entityStorage->GetEntityCount(); i++)
		{
			Entity* entity = entityStorage->GetEntity(i);
			if (entity != nullptr)
			{
				TransformComponent* transform = WorldEntityManager->GetComponent<TransformComponent>(entity);
				TriggerComponent* trigger = WorldEntityManager->GetComponent<TriggerComponent>(entity);

				if (transform != nullptr && trigger != nullptr)
				{
					float speed = 50.0f;
					float angle = trigger->CurrentAngle;
					if (angle > 360.0f) angle -= 360.0f;

					angle += speed * deltaTime;
					float radius = trigger->Radius;
					float x = radius * glm::sin(glm::radians(angle));
					float y = radius * glm::cos(glm::radians(angle));

					transform->Position.x = trigger->MiddlePoint.x + x;
					transform->Position.z = trigger->MiddlePoint.z + y;
					trigger->CurrentAngle = angle;

					CameraEntity* RenderCamera = CameraSystem->GetMainCamera();
					TransformComponent* cameraTransformComponent = WorldEntityManager->GetComponent<TransformComponent>(RenderCamera);
					CameraComponent* cameraComponent = WorldEntityManager->GetComponent<CameraComponent>(RenderCamera);
					
					if (glm::distance(cameraTransformComponent->Position, transform->Position) < (transform->Scale.x * 1.1f))
					{
						WorldEntityManager->MarkEntityForDeletion(entity->GetID(), entity->GetType());
						GameScript->AddPoint();
					}
				}
			}
		}
	}
	inline static const std::string SystemName = "TriggerSystem";
	inline static const System::SystemType SystemType = System::SystemType::System;

private:
	CameraControllerSystem* CameraSystem;
	CollectingGame* GameScript;
};
