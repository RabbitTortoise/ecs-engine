#pragma once

#include <Engine/ECS/Entity.h>
#include <Engine/ECS/EntityTypeStorage.h>
#include <Engine/ECS/EntityManager.h>
#include <Engine/OpenGL/ECS/Components/MaterialComponent.h>
#include <Engine/OpenGL/ECS/Components/MeshComponent.h>
#include <Engine/ECS/Components/TransformComponent.h>
#include <Game/Components/GameStatComponent.h>
#include <Game/Components/PlayerComponent.h>
#include <Game/Components/CarryComponent.h>


class CollectorEntity : public Entity
{
	friend class EntityTypeStorage<CollectorEntity>;

public:
	CollectorEntity() : Entity(-1, -1, false) {};
	CollectorEntity(int id, int type, bool enabled = true) : Entity(id, type, enabled) {}
	~CollectorEntity() {}

	//To make it Vector compatible:
	CollectorEntity(CollectorEntity const& e) :
		Transform(e.Transform), 
		Material(e.Material) ,
		Mesh(e.Mesh),
		Stats(e.Stats),
		Player(e.Player),
		Carry(e.Carry),
		Entity(e) 
	{}

	CollectorEntity(CollectorEntity&& e) noexcept :
		Transform(std::move(e.Transform)), 
		Material(std::move(e.Material)),
		Mesh(std::move(e.Mesh)),
		Stats(std::move(e.Stats)),
		Player(std::move(e.Player)),
		Carry(std::move(e.Carry)),
		Entity(std::move(e)) 
	{}
	CollectorEntity& operator=(const CollectorEntity& e)
	{ 
		Transform = e.Transform;
		Material = e.Material;
		Mesh = e.Mesh;
		Stats = e.Stats;
		Player = e.Player;
		Carry = e.Carry;
		return *this;
	}


	Component* GetComponent(int componentIndex) override
	{
		if (componentIndex == TransformComponent::ComponentIndex) { return &Transform; }
		if (componentIndex == MaterialComponent::ComponentIndex) { return &Material; }
		if (componentIndex == MeshComponent::ComponentIndex) { return &Mesh; }
		if (componentIndex == GameStatComponent::ComponentIndex) { return &Stats; }
		if (componentIndex == PlayerComponent::ComponentIndex) { return &Player; }
		if (componentIndex == CarryComponent::ComponentIndex) { return &Carry; }
		return nullptr;
	}

	static std::string GetEntityName() { return EntityName; }
	static std::vector<std::string> GetComponentTypeNames() { return ComponentTypeNames; }


	TransformComponent Transform;
	MaterialComponent Material;
	MeshComponent Mesh;
	GameStatComponent Stats;
	PlayerComponent Player;
	CarryComponent Carry;

private:
	inline static const std::string EntityName = "CollectorEntity";
	inline static const std::vector<std::string> ComponentTypeNames = { "TransformComponent", "MaterialComponent", "MeshComponent", "GameStatComponent", "PlayerComponent", "CarryComponent"};
};

