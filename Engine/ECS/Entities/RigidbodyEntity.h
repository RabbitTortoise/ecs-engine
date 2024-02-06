#pragma once

#include <Engine/ECS/Entity.h>
#include <Engine/ECS/EntityTypeStorage.h>
#include <Engine/ECS/EntityManager.h>

#include <Engine/ECS/Components/TransformComponent.h>
#include <Engine/ECS/Components/BoxColliderComponent.h>
#include <Engine/ECS/Components/RigidbodyComponent.h>
#include <Engine/OpenGL/ECS/Components/MaterialComponent.h>
#include <Engine/OpenGL/ECS/Components/MeshComponent.h>

class RigidbodyEntity : public Entity
{

	friend class EntityTypeStorage<RigidbodyEntity>;

public:
	RigidbodyEntity() : Entity(-1, -1, false) 
	{
	}
	RigidbodyEntity(int id, int type, bool enabled = true) : Entity(id, type, enabled) 
	{
	}
	~RigidbodyEntity() {}

	//To make it Vector compatible:
	RigidbodyEntity(RigidbodyEntity const& e) : 
		Transform(e.Transform), 
		BoxCollider(e.BoxCollider) ,
		RigidBody(e.RigidBody),
		Material(e.Material),
		Mesh(e.Mesh),
		Entity(e) {}

	RigidbodyEntity(RigidbodyEntity&& e) noexcept : 
		Transform(std::move(e.Transform)), 
		BoxCollider(std::move(e.BoxCollider)),
		RigidBody(std::move(e.RigidBody)),
		Material(std::move(e.Material)),
		Mesh(std::move(e.Mesh)),
		Entity(std::move(e)) {}

	RigidbodyEntity& operator=(const RigidbodyEntity& e)
	{
		Transform = e.Transform;
		BoxCollider = e.BoxCollider;
		RigidBody = e.RigidBody;
		Material = e.Material;
		Mesh = e.Mesh;
		return *this;
	}

	Component* GetComponent(int componentIndex) override
	{
		if (componentIndex == TransformComponent::ComponentIndex) { return &Transform; }
		if (componentIndex == BoxColliderComponent::ComponentIndex) { return &BoxCollider; }
		if (componentIndex == RigidbodyComponent::ComponentIndex) { return &RigidBody; }
		if (componentIndex == MaterialComponent::ComponentIndex) { return &Material; }
		if (componentIndex == MeshComponent::ComponentIndex) { return &Mesh; }
		return nullptr;
	}

	static std::string GetEntityName() { return EntityName; }
	static std::vector<std::string> GetComponentTypeNames() { return ComponentTypeNames; }

private:
	TransformComponent Transform;
	BoxColliderComponent BoxCollider;
	RigidbodyComponent RigidBody;
	MaterialComponent Material;
	MeshComponent Mesh;

	inline static const std::string EntityName = "RigidbodyEntity";
	inline static const std::vector<std::string> ComponentTypeNames = { "TransformComponent", "BoxColliderComponent", "RigidbodyComponent", "MaterialComponent", "MeshComponent" };
};

