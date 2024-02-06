#pragma once

#include <Engine/ECS/System.h>
#include <Engine/ECS/Components/TransformComponent.h>
#include <Game/Components/GameStatComponent.h>
#include <Game/Components/PlayerComponent.h>
#include <Game/Components/CarryComponent.h>
#include <iostream>
#include <Game/Scripts/AICourseGameManager.h>
#include <Game/Entities/TowerEntity.h>
#include <Game/Entities/AttackerEntity.h>
#include <Game/Scripts/Astar.h>
#include <deque>


class AttackerAgentSystem : public IComponentTypeQuerySystem
{

	struct NodeGroup
	{
		std::vector<AStar::AStarNode<AICourseAStar::State, float>*> OpenList;
		std::vector<AStar::AStarNode<AICourseAStar::State, float>*> ClosedList;
		AICourseAStar::State GoalState;
		std::deque<AICourseAStar::State> Path;
		bool InUse;
	};



public:
	AttackerAgentSystem(EntityManager* entityManager, AICourseGameManager* gameManager) : IComponentTypeQuerySystem(entityManager)
	{
		GameManager = gameManager;
		WorldEntityManager = entityManager;
	}

	void Init();

	void Destroy();

	void PreUpdate(float deltaTime);

	void ComponentTypeQueryUpdate(float deltaTime, IEntityStorage* entityTypeStorage);

	void FindPathToTower(AttackerEntity* attacker, TowerEntity* tower);

	void CalculatePathVector(AttackerEntity* attacker, AStar::AStarNode<AICourseAStar::State, float>* closestNode);


	int FindNearestEnemyTower(AttackerEntity* attacker);
	int FindNearestOwnedTower(AttackerEntity* attacker);
	

	float RandomNumber0_1()
	{
		return (static_cast <float> (rand()) / static_cast <float> (RAND_MAX));
	}

	


	inline static const std::string SystemName = "AttackerAgentSystem";
	inline static const System::SystemType SystemType = System::SystemType::System;

private:
	AICourseGameManager* GameManager;
	EntityManager* WorldEntityManager;

	std::vector<NodeGroup*> NodeGroups;
};
