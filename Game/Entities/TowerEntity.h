#pragma once

#include <Engine/ECS/Entity.h>
#include <Engine/ECS/EntityTypeStorage.h>
#include <Engine/ECS/EntityManager.h>
#include <Engine/OpenGL/ECS/Components/MaterialComponent.h>
#include <Engine/OpenGL/ECS/Components/MeshComponent.h>
#include <Engine/ECS/Components/TransformComponent.h>
#include <Game/Components/GameStatComponent.h>
#include <Game/Components/PlayerComponent.h>


class TowerEntity : public Entity
{
	friend class EntityTypeStorage<TowerEntity>;

public:
	TowerEntity() : Entity(-1, -1, false) {};
	TowerEntity(int id, int type, bool enabled = true) : Entity(id, type, enabled) {}
	~TowerEntity() {}

	//To make it Vector compatible:
	TowerEntity(TowerEntity const& e) :
		Transform(e.Transform), 
		Material(e.Material) ,
		Mesh(e.Mesh),
		Stats(e.Stats),
		Player(e.Player),
		Entity(e) 
	{}

	TowerEntity(TowerEntity&& e) noexcept :
		Transform(std::move(e.Transform)), 
		Material(std::move(e.Material)),
		Mesh(std::move(e.Mesh)),
		Stats(std::move(e.Stats)),
		Player(std::move(e.Player)),
		Entity(std::move(e)) 
	{}
	TowerEntity& operator=(const TowerEntity& e)
	{ 
		Transform = e.Transform;
		Material = e.Material;
		Mesh = e.Mesh;
		Stats = e.Stats;
		Player = e.Player;
		return *this;
	}


	Component* GetComponent(int componentIndex) override
	{
		if (componentIndex == TransformComponent::ComponentIndex) { return &Transform; }
		if (componentIndex == MaterialComponent::ComponentIndex) { return &Material; }
		if (componentIndex == MeshComponent::ComponentIndex) { return &Mesh; }
		if (componentIndex == GameStatComponent::ComponentIndex) { return &Stats; }
		if (componentIndex == PlayerComponent::ComponentIndex) { return &Player; }
		return nullptr;
	}

	static std::string GetEntityName() { return EntityName; }
	static std::vector<std::string> GetComponentTypeNames() { return ComponentTypeNames; }


	TransformComponent Transform;
	MaterialComponent Material;
	MeshComponent Mesh;
	GameStatComponent Stats;
	PlayerComponent Player;

private:
	inline static const std::string EntityName = "TowerEntity";
	inline static const std::vector<std::string> ComponentTypeNames = { "TransformComponent", "MaterialComponent", "MeshComponent", "GameStatComponent", "PlayerComponent"};
};

