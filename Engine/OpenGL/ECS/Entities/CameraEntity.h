#pragma once

#include <Engine/ECS/Entity.h>
#include <Engine/ECS/EntityTypeStorage.h>
#include <Engine/ECS/EntityManager.h>
#include <Engine/ECS/Components/TransformComponent.h>
#include <Engine/OpenGL/ECS/Components/CameraComponent.h>
#include <Engine/ECS/Components/MovementComponent.h>


class CameraEntity : public Entity
{

	friend class EntityTypeStorage<CameraEntity>;

public:
	CameraEntity() : Entity(-1, -1, false) 
	{
	}
	CameraEntity(int id, int type, bool enabled = true) : Entity(id, type, enabled) 
	{
	}
	~CameraEntity() {}

	//To make it Vector compatible:
	CameraEntity(CameraEntity const& e) : 
		Transform(e.Transform), 
		Camera(e.Camera) ,
		Movement(e.Movement), 
		Entity(e) {}

	CameraEntity(CameraEntity&& e) noexcept : 
		Transform(std::move(e.Transform)), 
		Camera(std::move(e.Camera)),
		Movement(std::move(e.Movement)),
		Entity(std::move(e)) {}

	CameraEntity& operator=(const CameraEntity& e)
	{
		Transform = e.Transform;
		Camera = e.Camera;
		Movement = e.Movement;
		return *this;
	}

	Component* GetComponent(int componentIndex) override
	{
		if (componentIndex == TransformComponent::ComponentIndex) { return &Transform; }
		if (componentIndex == CameraComponent::ComponentIndex) { return &Camera; }
		if (componentIndex == MovementComponent::ComponentIndex) { return &Movement; }
		return nullptr;
	}

	static std::string GetEntityName() { return EntityName; }
	static std::vector<std::string> GetComponentTypeNames() { return ComponentTypeNames; }

private:
	TransformComponent Transform;
	CameraComponent Camera;
	MovementComponent Movement;

	inline static const std::string EntityName = "CameraEntity";
	inline static const std::vector<std::string> ComponentTypeNames = { "TransformComponent", "CameraComponent", "MovementComponent" };
};

