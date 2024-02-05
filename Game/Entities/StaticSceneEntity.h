#pragma once


#include <Engine/ECS/Entity.h>
#include <Engine/ECS/EntityTypeStorage.h>
#include <Engine/ECS/EntityManager.h>
#include <Engine/OpenGL/ECS/Components/MaterialComponent.h>
#include <Engine/OpenGL/ECS/Components/MeshComponent.h>
#include <Engine/ECS/Components/TransformComponent.h>


class StaticSceneEntity : public Entity
{
	friend class EntityTypeStorage<StaticSceneEntity>;

public:
	StaticSceneEntity() : Entity(-1, -1, false) {};
	StaticSceneEntity(int id, int type, bool enabled = true) : Entity(id, type, enabled) {}
	~StaticSceneEntity() {}

	//To make it Vector compatible:
	StaticSceneEntity(StaticSceneEntity const& e) :
		Transform(e.Transform), 
		Material(e.Material) ,
		Mesh(e.Mesh),
		Entity(e) 
	{}

	StaticSceneEntity(StaticSceneEntity&& e) noexcept :
		Transform(std::move(e.Transform)), 
		Material(std::move(e.Material)),
		Mesh(std::move(e.Mesh)),
		Entity(std::move(e)) 
	{}
	StaticSceneEntity& operator=(const StaticSceneEntity& e)
	{ 
		Transform = e.Transform;
		Material = e.Material;
		Mesh = e.Mesh;
		return *this;
	}
	Component* GetComponent(int componentIndex) override
	{
		if (componentIndex == TransformComponent::ComponentIndex) { return &Transform; }
		if (componentIndex == MaterialComponent::ComponentIndex) { return &Material; }
		if (componentIndex == MeshComponent::ComponentIndex) { return &Mesh; }
		return nullptr;
	}

	static std::string GetEntityName() { return EntityName; }
	static std::vector<std::string> GetComponentTypeNames() { return ComponentTypeNames; }

private:
	TransformComponent Transform;
	MaterialComponent Material;
	MeshComponent Mesh;

	inline static const std::string EntityName = "StaticSceneEntity";
	inline static const std::vector<std::string> ComponentTypeNames = { "TransformComponent", "MaterialComponent", "MeshComponent" };
};