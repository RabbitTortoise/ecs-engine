#pragma once

#include <Engine/ECS/Entity.h>
#include <Engine/ECS/EntityTypeStorage.h>
#include <Engine/ECS/EntityManager.h>
#include <Game/Components/TriggerComponent.h>
#include <Engine/OpenGL/ECS/Components/MaterialComponent.h>
#include <Engine/OpenGL/ECS/Components/MeshComponent.h>
#include <Engine/ECS/Components/TransformComponent.h>


class MovingTriggerEntity : public Entity
{
	friend class EntityTypeStorage
		<MovingTriggerEntity>;

public:
	MovingTriggerEntity() : Entity(-1, -1, false) {};
	MovingTriggerEntity(int id, int type, bool enabled = true) : Entity(id, type, enabled) {}
	~MovingTriggerEntity() {}

	//To make it Vector compatible:
	MovingTriggerEntity(MovingTriggerEntity const& e) :
		Transform(e.Transform), 
		Material(e.Material) ,
		Mesh(e.Mesh),
		Trigger(e.Trigger),
		Entity(e) 
	{}

	MovingTriggerEntity(MovingTriggerEntity&& e) noexcept :
		Transform(std::move(e.Transform)), 
		Material(std::move(e.Material)),
		Mesh(std::move(e.Mesh)),
		Trigger(std::move(e.Trigger)),
		Entity(std::move(e)) 
	{}
	MovingTriggerEntity& operator=(const MovingTriggerEntity& e)
	{ 
		Transform = e.Transform;
		Material = e.Material;
		Mesh = e.Mesh;
		Trigger = e.Trigger;
		return *this;
	}

	Component* GetComponent(int componentIndex) override
	{
		if (componentIndex == TransformComponent::ComponentIndex) { return &Transform; }
		if (componentIndex == MaterialComponent::ComponentIndex) { return &Material; }
		if (componentIndex == MeshComponent::ComponentIndex) { return &Mesh; }
		if (componentIndex == TriggerComponent::ComponentIndex) { return &Trigger; }
		return nullptr;
	}

	static std::string GetEntityName() { return EntityName; }
	static std::vector<std::string> GetComponentTypeNames() { return ComponentTypeNames; }

private:
	TransformComponent Transform;
	MaterialComponent Material;
	MeshComponent Mesh;
	TriggerComponent Trigger;

	inline static const std::string EntityName = "MovingTriggerEntity";
	inline static const std::vector<std::string> ComponentTypeNames = { "TransformComponent", "MaterialComponent", "MeshComponent" , "TriggerComponent" };
};
