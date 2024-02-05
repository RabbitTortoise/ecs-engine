#pragma once

#include <Engine/ECS/System.h>
#include <Engine/ECS/Components/TransformComponent.h>
#include <iostream>

class EntityRotateSystem : public IComponentTypeQuerySystem
{
public:
	EntityRotateSystem(EntityManager* entityManager) : IComponentTypeQuerySystem(entityManager)
	{
	}

	void Init()
	{
	}

	void Destroy()
	{
	}

	void PreUpdate(float deltaTime)
	{
	}

	void ComponentTypeQueryUpdate(float deltaTime, IEntityStorage* entityTypeStorage)
	{
		
		EntityTypeStorage<Entity>* entityStorage = static_cast<EntityTypeStorage<Entity>*>(entityTypeStorage);
		//std::cout << "EntityMovementSystem: Entity count: " << std::to_string(entityStorage->GetEntityCount()) << std::endl;

		for (int i = 0; i < entityStorage->GetEntityCount(); i++)
		{
			Entity* e = entityStorage->GetEntity(i);
			if (e != nullptr)
			{
				//TransformComponent* transform = e->GetComponent<TransformComponent>(WorldEntityManager->GetComponentTypeIdByName("TransformComponent"));
				TransformComponent* transform = WorldEntityManager->GetComponent<TransformComponent>(e);
				if (transform != nullptr)
				{
					transform->EulerAngles.x += 1.0f * deltaTime * RandomNumber0_1();
					transform->EulerAngles.y += 1.0f * deltaTime * RandomNumber0_1();
					transform->EulerAngles.z += 1.0f * deltaTime * RandomNumber0_1();
				}
			}
		}
	}

	float RandomNumber0_1()
	{
		return (static_cast <float> (rand()) / static_cast <float> (RAND_MAX));
	}


	inline static const std::string SystemName = "EntityRotateSystem";
	inline static const System::SystemType SystemType = System::SystemType::System;
};
