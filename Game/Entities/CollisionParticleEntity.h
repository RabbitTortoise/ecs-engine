#pragma once

#include <Engine/ECS/Entity.h>
#include <Engine/ECS/EntityTypeStorage.h>
#include <Engine/ECS/EntityManager.h>
#include <Engine/OpenGL/ECS/Components/MaterialComponent.h>
#include <Engine/OpenGL/ECS/Components/MeshComponent.h>
#include <Engine/ECS/Components/TransformComponent.h>
#include <Engine/ECS/Components/RigidbodyComponent.h>
#include <Engine/ECS/Components/BoxColliderComponent.h>


class CollisionParticleEntity : public Entity
{
	friend class EntityTypeStorage<CollisionParticleEntity>;

public:
	CollisionParticleEntity() : Entity(-1, -1, false) {};
	CollisionParticleEntity(int id, int type, bool enabled = true) : Entity(id, type, enabled) {}
	~CollisionParticleEntity() {}

	//To make it Vector compatible:
	CollisionParticleEntity(CollisionParticleEntity const& e) :
		Transform(e.Transform), 
		Material(e.Material) ,
		Mesh(e.Mesh),
		BoxCollider(e.BoxCollider),
		RigidBody(e.RigidBody),
		Entity(e) 
	{}

	CollisionParticleEntity(CollisionParticleEntity&& e) noexcept :
		Transform(std::move(e.Transform)), 
		Material(std::move(e.Material)),
		Mesh(std::move(e.Mesh)),
		BoxCollider(std::move(e.BoxCollider)),
		RigidBody(std::move(e.RigidBody)),
		Entity(std::move(e)) 
	{}
	CollisionParticleEntity& operator=(const CollisionParticleEntity& e)
	{ 
		Transform = e.Transform;
		Material = e.Material;
		Mesh = e.Mesh;
		BoxCollider = e.BoxCollider;
		RigidBody = e.RigidBody;
		return *this;
	}


	Component* GetComponent(int componentIndex) override
	{
		if (componentIndex == TransformComponent::ComponentIndex) { return &Transform; }
		if (componentIndex == MaterialComponent::ComponentIndex) { return &Material; }
		if (componentIndex == MeshComponent::ComponentIndex) { return &Mesh; }
		if (componentIndex == BoxColliderComponent::ComponentIndex) { return &BoxCollider; }
		if (componentIndex == RigidbodyComponent::ComponentIndex) { return &RigidBody; }
		return nullptr;
	}

	static std::string GetEntityName() { return EntityName; }
	static std::vector<std::string> GetComponentTypeNames() { return ComponentTypeNames; }

private:
	TransformComponent Transform;
	MaterialComponent Material;
	MeshComponent Mesh;
	BoxColliderComponent BoxCollider;
	RigidbodyComponent RigidBody;

	inline static const std::string EntityName = "CollisionParticleEntity";
	inline static const std::vector<std::string> ComponentTypeNames = { "TransformComponent", "MaterialComponent", "MeshComponent", "BoxColliderComponent", "RigidbodyComponent" };
};

