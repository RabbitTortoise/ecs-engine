#pragma once

#include <Engine/ECS/Entity.h>
#include <Engine/ECS/EntityTypeStorage.h>
#include <Engine/ECS/EntityManager.h>
#include <Engine/OpenGL/ECS/Components/MaterialComponent.h>
#include <Engine/OpenGL/ECS/Components/MeshComponent.h>
#include <Engine/ECS/Components/TransformComponent.h>
#include <Game/Components/GameStatComponent.h>
#include <Game/Components/PlayerComponent.h>


class GoldEntity : public Entity
{
	friend class EntityTypeStorage<GoldEntity>;

public:
	GoldEntity() : Entity(-1, -1, false) {};
	GoldEntity(int id, int type, bool enabled = true) : Entity(id, type, enabled) {}
	~GoldEntity() {}

	//To make it Vector compatible:
	GoldEntity(GoldEntity const& e) :
		Transform(e.Transform), 
		Material(e.Material) ,
		Mesh(e.Mesh),
		Stats(e.Stats),
		Entity(e) 
	{}

	GoldEntity(GoldEntity&& e) noexcept :
		Transform(std::move(e.Transform)), 
		Material(std::move(e.Material)),
		Mesh(std::move(e.Mesh)),
		Stats(std::move(e.Stats)),
		Entity(std::move(e)) 
	{}
	GoldEntity& operator=(const GoldEntity& e)
	{ 
		Transform = e.Transform;
		Material = e.Material;
		Mesh = e.Mesh;
		Stats = e.Stats;
		return *this;
	}


	Component* GetComponent(int componentIndex) override
	{
		if (componentIndex == TransformComponent::ComponentIndex) { return &Transform; }
		if (componentIndex == MaterialComponent::ComponentIndex) { return &Material; }
		if (componentIndex == MeshComponent::ComponentIndex) { return &Mesh; }
		if (componentIndex == GameStatComponent::ComponentIndex) { return &Stats; }
		return nullptr;
	}

	static std::string GetEntityName() { return EntityName; }
	static std::vector<std::string> GetComponentTypeNames() { return ComponentTypeNames; }


	TransformComponent Transform;
	MaterialComponent Material;
	MeshComponent Mesh;
	GameStatComponent Stats;

private:
	inline static const std::string EntityName = "GoldEntity";
	inline static const std::vector<std::string> ComponentTypeNames = { "TransformComponent", "MaterialComponent", "MeshComponent", "GameStatComponent"};
};

