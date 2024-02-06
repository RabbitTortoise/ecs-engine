#pragma once

#include <Engine/ECS/System.h>			// System declaration
#include <map>


class PhysicsSystem : public IComponentTypeQuerySystem
{
public:

	PhysicsSystem(EntityManager* entityManager);
	~PhysicsSystem();
	
	//System class overrides
	void Init() override;
	void Destroy() override;
	void PreUpdate(float deltaTime) override;
	void ComponentTypeQueryUpdate(float deltaTime, IEntityStorage* entityTypeStorage) override;

	

	inline static const std::string SystemName = "PhysicsSystem";
	inline static const System::SystemType SystemType = System::SystemType::CollisionSystem;
private:
	
	std::map<int, int> DetectedCollisions;

	EntityManager* WorldEntityManager;
};
