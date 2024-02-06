#include <PhysicsSystem.h>
#include <Engine/ECS/Components/TransformComponent.h>
#include <Engine/ECS/Components/BoxColliderComponent.h>
#include <Engine/ECS/Components/RigidbodyComponent.h>
#include <Engine/Core/MatrixCalculations.h>


PhysicsSystem::PhysicsSystem(EntityManager* entityManager) : IComponentTypeQuerySystem(entityManager)
{
	WorldEntityManager = entityManager;
}

PhysicsSystem::~PhysicsSystem()
{
	Destroy();
}

void PhysicsSystem::Init()
{
	
}

void PhysicsSystem::Destroy()
{
	
}

void PhysicsSystem::PreUpdate(float deltaTime)
{
	
}




// check if there's a separating plane in between the selected axes
bool GetSeparatingPlane(const glm::vec3& RPos, const glm::vec3& Plane, const BoxColliderComponent* collider_A, const BoxColliderComponent* collider_B )
{
	/*float f1 = fabs(glm::dot(RPos, Plane));

	float f21 = fabs(glm::dot((collider_A->AxisX * collider_A->BoxHalfSize.x), Plane));
	float f22 = fabs(glm::dot((collider_A->AxisY * collider_A->BoxHalfSize.y), Plane));
	float f23 = fabs(glm::dot((collider_A->AxisZ * collider_A->BoxHalfSize.z), Plane));
	float f24 = fabs(glm::dot((collider_B->AxisX * collider_B->BoxHalfSize.x), Plane));
	float f25 = fabs(glm::dot((collider_B->AxisY * collider_B->BoxHalfSize.y), Plane));
	float f26 = fabs(glm::dot((collider_B->AxisZ * collider_B->BoxHalfSize.z), Plane));

	float f2 =
		fabs(glm::dot((collider_A->AxisX * collider_A->BoxHalfSize.x), Plane)) +
		fabs(glm::dot((collider_A->AxisY * collider_A->BoxHalfSize.y), Plane)) +
		fabs(glm::dot((collider_A->AxisZ * collider_A->BoxHalfSize.z), Plane)) +
		fabs(glm::dot((collider_B->AxisX * collider_B->BoxHalfSize.x), Plane)) +
		fabs(glm::dot((collider_B->AxisY * collider_B->BoxHalfSize.y), Plane)) +
		fabs(glm::dot((collider_B->AxisZ * collider_B->BoxHalfSize.z), Plane));*/


	return (	
		fabs(glm::dot(RPos, Plane)) >
		(
		fabs(glm::dot((collider_A->AxisX * collider_A->BoxHalfSize.x), Plane)) +
		fabs(glm::dot((collider_A->AxisY * collider_A->BoxHalfSize.y), Plane)) +
		fabs(glm::dot((collider_A->AxisZ * collider_A->BoxHalfSize.z), Plane)) +
		fabs(glm::dot((collider_B->AxisX * collider_B->BoxHalfSize.x), Plane)) +
		fabs(glm::dot((collider_B->AxisY * collider_B->BoxHalfSize.y), Plane)) +
		fabs(glm::dot((collider_B->AxisZ * collider_B->BoxHalfSize.z), Plane))
		)
		);
}

// test for separating planes in all 15 axes
bool GetCollision
(
	const BoxColliderComponent* collider_A,
	const BoxColliderComponent* collider_B,
	TransformComponent* transform_A,
	TransformComponent* transform_B)
{
	glm::vec3 RPos = transform_B->Position - transform_A->Position;
	
	/*bool s1 = GetSeparatingPlane(RPos, collider_A->AxisX, collider_A, collider_B);
	bool s2 = GetSeparatingPlane(RPos, collider_A->AxisY, collider_A, collider_B);
	bool s3 = GetSeparatingPlane(RPos, collider_A->AxisZ, collider_A, collider_B);
	bool s4 = GetSeparatingPlane(RPos, collider_B->AxisX, collider_A, collider_B);
	bool s5 = GetSeparatingPlane(RPos, collider_B->AxisY, collider_A, collider_B);
	bool s6 = GetSeparatingPlane(RPos, collider_B->AxisZ, collider_A, collider_B);
	bool s7 = GetSeparatingPlane(RPos, glm::cross(collider_A->AxisX, collider_B->AxisX), collider_A, collider_B);
	bool s8 = GetSeparatingPlane(RPos, glm::cross(collider_A->AxisX, collider_B->AxisY), collider_A, collider_B);
	bool s9 = GetSeparatingPlane(RPos, glm::cross(collider_A->AxisX, collider_B->AxisZ), collider_A, collider_B);
	bool s10 = GetSeparatingPlane(RPos, glm::cross(collider_A->AxisY, collider_B->AxisX), collider_A, collider_B);
	bool s11 = GetSeparatingPlane(RPos, glm::cross(collider_A->AxisY, collider_B->AxisY), collider_A, collider_B);
	bool s12 = GetSeparatingPlane(RPos, glm::cross(collider_A->AxisY, collider_B->AxisZ), collider_A, collider_B);
	bool s13 = GetSeparatingPlane(RPos, glm::cross(collider_A->AxisZ, collider_B->AxisX), collider_A, collider_B);
	bool s14 = GetSeparatingPlane(RPos, glm::cross(collider_A->AxisZ, collider_B->AxisY), collider_A, collider_B);
	bool s15 = GetSeparatingPlane(RPos, glm::cross(collider_A->AxisZ, collider_B->AxisZ), collider_A, collider_B);
	bool s16 = false;*/


	return !(
		GetSeparatingPlane(RPos, collider_A->AxisX, collider_A, collider_B) ||
		GetSeparatingPlane(RPos, collider_A->AxisY, collider_A, collider_B) ||
		GetSeparatingPlane(RPos, collider_A->AxisZ, collider_A, collider_B) ||
		GetSeparatingPlane(RPos, collider_B->AxisX, collider_A, collider_B) ||
		GetSeparatingPlane(RPos, collider_B->AxisY, collider_A, collider_B) ||
		GetSeparatingPlane(RPos, collider_B->AxisZ, collider_A, collider_B) ||
		GetSeparatingPlane(RPos, glm::cross(collider_A->AxisX, collider_B->AxisX), collider_A, collider_B) ||
		GetSeparatingPlane(RPos, glm::cross(collider_A->AxisX, collider_B->AxisY), collider_A, collider_B) ||
		GetSeparatingPlane(RPos, glm::cross(collider_A->AxisX, collider_B->AxisZ), collider_A, collider_B) ||
		GetSeparatingPlane(RPos, glm::cross(collider_A->AxisY, collider_B->AxisX), collider_A, collider_B) ||
		GetSeparatingPlane(RPos, glm::cross(collider_A->AxisY, collider_B->AxisY), collider_A, collider_B) ||
		GetSeparatingPlane(RPos, glm::cross(collider_A->AxisY, collider_B->AxisZ), collider_A, collider_B) ||
		GetSeparatingPlane(RPos, glm::cross(collider_A->AxisZ, collider_B->AxisX), collider_A, collider_B) ||
		GetSeparatingPlane(RPos, glm::cross(collider_A->AxisZ, collider_B->AxisY), collider_A, collider_B) ||
		GetSeparatingPlane(RPos, glm::cross(collider_A->AxisZ, collider_B->AxisZ), collider_A, collider_B));
}


void PhysicsSystem::ComponentTypeQueryUpdate(float deltaTime, IEntityStorage* entityTypeStorage)
{

	EntityTypeStorage<Entity>* entityStorage = static_cast<EntityTypeStorage<Entity>*>(entityTypeStorage);
	int entityType = entityStorage->GetEntityType();
	int entityCount = entityStorage->GetEntityCount();

	//Update collider axes
	for (int i = 0; i < entityStorage->GetEntityCount(); i++)
	{
		Entity* entity_A = entityStorage->GetEntity(i);
		if (entity_A != nullptr)
		{
			BoxColliderComponent* boxColliderComponent_A = WorldEntityManager->GetComponent<BoxColliderComponent>(entity_A);
			TransformComponent* transformComponent_A = WorldEntityManager->GetComponent<TransformComponent>(entity_A);

			boxColliderComponent_A->AxisX = MatrixCalculations::CalculateRightVector(transformComponent_A->Orientation);
			boxColliderComponent_A->AxisY = MatrixCalculations::CalculateUpVector(transformComponent_A->Orientation);
			boxColliderComponent_A->AxisZ = MatrixCalculations::CalculateForwardVector(transformComponent_A->Orientation);
		}
	}

	//Check for Collisions
	for (int i = 0; i < entityStorage->GetEntityCount(); i++)
	{
		Entity* entity_A = entityStorage->GetEntity(i);
		if (entity_A != nullptr)
		{
			BoxColliderComponent* boxColliderComponent_A = WorldEntityManager->GetComponent<BoxColliderComponent>(entity_A);
			TransformComponent* transformComponent_A = WorldEntityManager->GetComponent<TransformComponent>(entity_A);

			
			for (int i = 0; i < entityStorage->GetEntityCount(); i++)
			{
			
				Entity* entity_B = entityStorage->GetEntity(i);
				if (entity_B != nullptr && entity_A != entity_B)
				{
					BoxColliderComponent* boxColliderComponent_B = WorldEntityManager->GetComponent<BoxColliderComponent>(entity_B);
					TransformComponent* transformComponent_B = WorldEntityManager->GetComponent<TransformComponent>(entity_B);

					bool collisionDetected = GetCollision(boxColliderComponent_A, boxColliderComponent_B, transformComponent_A, transformComponent_B);
					if (collisionDetected)
					{
						DetectedCollisions.insert({ entity_A->GetID(),entity_B->GetID() });
					}
				}
			}
		}
	}

	//Apply Gravity
	for (int i = 0; i < entityStorage->GetEntityCount(); i++)
	{
		Entity* entity_A = entityStorage->GetEntity(i);
		if (entity_A != nullptr)
		{
			BoxColliderComponent* boxColliderComponent_A = WorldEntityManager->GetComponent<BoxColliderComponent>(entity_A);
			TransformComponent* transformComponent_A = WorldEntityManager->GetComponent<TransformComponent>(entity_A);
			RigidbodyComponent* rigidbodyComponent_A = WorldEntityManager->GetComponent<RigidbodyComponent>(entity_A);

			//Apply Gravity

			if (rigidbodyComponent_A->AffectedByGravity)
			{
				rigidbodyComponent_A->Velocity += rigidbodyComponent_A->Gravity * deltaTime;
			}
		}
	}
	
	//Apply Collision forces
	for (auto& pair : DetectedCollisions)
	{
		Entity* entity_A = entityTypeStorage->GetEntity(pair.first);
		Entity* entity_B = entityTypeStorage->GetEntity(pair.second);

		TransformComponent* transformComponent_A = WorldEntityManager->GetComponent<TransformComponent>(entity_A);
		TransformComponent* transformComponent_B = WorldEntityManager->GetComponent<TransformComponent>(entity_B);

		BoxColliderComponent* boxColliderComponent_A = WorldEntityManager->GetComponent<BoxColliderComponent>(entity_A);
		BoxColliderComponent* boxColliderComponent_B = WorldEntityManager->GetComponent<BoxColliderComponent>(entity_B);

		RigidbodyComponent* rigidbodyComponent_A = WorldEntityManager->GetComponent<RigidbodyComponent>(entity_A);
		RigidbodyComponent* rigidbodyComponent_B = WorldEntityManager->GetComponent<RigidbodyComponent>(entity_B);

		rigidbodyComponent_A->Velocity = glm::vec3(0, 0, 0);
		rigidbodyComponent_B->Velocity = glm::vec3(0, 0, 0);

		//rigidbodyComponent_A->Velocity = rigidbodyComponent_A->Velocity * -1.0f * rigidbodyComponent_A->Restitution;
		//rigidbodyComponent_B->Velocity = rigidbodyComponent_B->Velocity * -1.0f * rigidbodyComponent_B->Restitution;

		/*
		glm::vec3 N = glm::normalize(transformComponent_B->Position - transformComponent_A->Position);
		glm::vec3 newVel = rigidbodyComponent_A->Restitution * glm::reflect(rigidbodyComponent_A->Velocity, N);
		rigidbodyComponent_A->Velocity = newVel;

		N = glm::normalize(transformComponent_B->Position - transformComponent_A->Position);
		newVel = rigidbodyComponent_A->Restitution * glm::reflect(rigidbodyComponent_A->Velocity, N);
		rigidbodyComponent_B->Velocity = newVel;
		*/
	}
	if(DetectedCollisions.size() < 1)
	{
		//std::cout << "NO COLLISION" << std::endl;
	}

	DetectedCollisions.clear();

	//Update position
	for (int i = 0; i < entityStorage->GetEntityCount(); i++)
	{
		Entity* entity_A = entityStorage->GetEntity(i);
		if (entity_A != nullptr)
		{
			BoxColliderComponent* boxColliderComponent_A = WorldEntityManager->GetComponent<BoxColliderComponent>(entity_A);
			TransformComponent* transformComponent_A = WorldEntityManager->GetComponent<TransformComponent>(entity_A);
			RigidbodyComponent* rigidbodyComponent_A = WorldEntityManager->GetComponent<RigidbodyComponent>(entity_A);

			//Update Position
			transformComponent_A->Position += rigidbodyComponent_A->Velocity * deltaTime;
		}
	}


	//Apply Physics
	//V1a = -eV1b
	//V1n = V
	//
	//glm::mat3 To
	
}
