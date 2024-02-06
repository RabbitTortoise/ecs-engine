#pragma once

#include <Engine/ECS/Entity.h>
#include <Engine/ECS/EntityTypeStorage.h>
#include <Engine/ECS/EntityManager.h>
#include <Engine/OpenGL/ECS/Components/MaterialComponent.h>
#include <Engine/OpenGL/ECS/Components/MeshComponent.h>
#include <Engine/ECS/Components/TransformComponent.h>
#include <Game/Components/GameStatComponent.h>
#include <Game/Components/PlayerComponent.h>
#include <Game/Components/AStarComponent.h>


class AttackerEntity : public Entity
{
	friend class EntityTypeStorage<AttackerEntity>;

public:
	AttackerEntity() : Entity(-1, -1, false) {};
	AttackerEntity(int id, int type, bool enabled = true) : Entity(id, type, enabled) {}
	~AttackerEntity() {}

	//To make it Vector compatible:
	AttackerEntity(AttackerEntity const& e) :
		Transform(e.Transform), 
		Material(e.Material) ,
		Mesh(e.Mesh),
		Stats(e.Stats),
		Player(e.Player),
		AStar(e.AStar),
		Entity(e) 
	{}

	AttackerEntity(AttackerEntity&& e) noexcept :
		Transform(std::move(e.Transform)), 
		Material(std::move(e.Material)),
		Mesh(std::move(e.Mesh)),
		Stats(std::move(e.Stats)),
		Player(std::move(e.Player)),
		AStar(std::move(e.AStar)),
		Entity(std::move(e)) 
	{}
	AttackerEntity& operator=(const AttackerEntity& e)
	{ 
		Transform = e.Transform;
		Material = e.Material;
		Mesh = e.Mesh;
		Stats = e.Stats;
		Player = e.Player;
		AStar = e.AStar;
		return *this;
	}


	Component* GetComponent(int componentIndex) override
	{
		if (componentIndex == TransformComponent::ComponentIndex) { return &Transform; }
		if (componentIndex == MaterialComponent::ComponentIndex) { return &Material; }
		if (componentIndex == MeshComponent::ComponentIndex) { return &Mesh; }
		if (componentIndex == GameStatComponent::ComponentIndex) { return &Stats; }
		if (componentIndex == PlayerComponent::ComponentIndex) { return &Player; }
		if (componentIndex == AStarComponent::ComponentIndex) { return &AStar; }
		return nullptr;
	}

	static std::string GetEntityName() { return EntityName; }
	static std::vector<std::string> GetComponentTypeNames() { return ComponentTypeNames; }


	TransformComponent Transform;
	MaterialComponent Material;
	MeshComponent Mesh;
	GameStatComponent Stats;
	PlayerComponent Player;
	AStarComponent AStar;

private:
	inline static const std::string EntityName = "AttackerEntity";
	inline static const std::vector<std::string> ComponentTypeNames = { "TransformComponent", "MaterialComponent", "MeshComponent", "GameStatComponent", "PlayerComponent", "AStarComponent" };
};

