#pragma once

#include <Game/Systems/AttackerAgentSystem.h>

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



void AttackerAgentSystem::Init()
{
}

void AttackerAgentSystem::Destroy()
{
	for (auto ng : NodeGroups)
	{
		delete ng;
		ng = nullptr;
	}
}

void AttackerAgentSystem::PreUpdate(float deltaTime)
{
}

void AttackerAgentSystem::ComponentTypeQueryUpdate(float deltaTime, IEntityStorage* entityTypeStorage)
{
	auto* entityStorage = WorldEntityManager->CastEntityStorage<AttackerEntity>(entityTypeStorage);

	for (int i = 0; i < entityStorage->GetEntityCount(); i++)
	{
		AttackerEntity* attacker = entityStorage->GetEntity(i);
		if (!attacker)
		{
			continue;
		}


		if (attacker->Stats.TargetEntity == -1)
		{
			attacker->Stats.TargetEntity = FindNearestEnemyTower(attacker);
			if (attacker->Stats.TargetEntity == -1)
			{
				continue; //No Tower Found
			}
		}
			
		TowerEntity* targetTower = WorldEntityManager->GetEntity<TowerEntity>(attacker->Stats.TargetEntity);
		if (!targetTower)
		{
			attacker->Stats.TargetEntity = -1;
			continue; //Tower target no longer valid;
		}

		//Check if close to target
		glm::vec3 distance = targetTower->Transform.Position - attacker->Transform.Position;
		distance.y = 0;

		if (glm::length(distance) < GameManager->GetAttackerInterractRadius())
		{
			if (targetTower->Stats.Health <= 0)
			{
				attacker->Stats.TargetEntity = -1;
				continue;
			}

			if (targetTower->Stats.Health < attacker->Stats.Health)
			{
				attacker->Stats.Health -= targetTower->Stats.Health;
				targetTower->Stats.Health = 0;

			}
			else
			{
				targetTower->Stats.Health -= attacker->Stats.Health;
				attacker->Stats.Health = 0;
			}
				
			if (targetTower->Stats.Health <= 0)
			{
				GameManager->RemoveTower(targetTower->Stats.GridPosition, targetTower->GetID());
			}
			if (attacker->Stats.Health <= 0)
			{
				int groupIndex = attacker->AStar.NodeGroupIndex;
				if (groupIndex > -1 && groupIndex < NodeGroups.size())
				{
					NodeGroups[groupIndex]->OpenList.clear();
					NodeGroups[groupIndex]->ClosedList.clear();
					NodeGroups[groupIndex]->Path.clear();;
					NodeGroups[groupIndex]->GoalState.Set({0, 0});
					NodeGroups[groupIndex]->InUse = false;

				}
				GameManager->RemoveAttacker(attacker->GetID());
			}


			continue;
		}

		if (attacker->AStar.NodeGroupIndex == -1)
		{
			FindPathToTower(attacker, targetTower);
			attacker->Stats.TargetGridPosition = ((NodeGroups[attacker->AStar.NodeGroupIndex]->Path.end())-1)->Get();
		}

		//Go to target tower

		//Check if close to path point;
		auto path = NodeGroups[attacker->AStar.NodeGroupIndex]->Path;
		int currentGoal = attacker->AStar.CurrentPathIndex + 1;
		if (currentGoal >= path.size())
		{
			
			int groupIndex = attacker->AStar.NodeGroupIndex;
			if (groupIndex > -1 && groupIndex < NodeGroups.size())
			{
				NodeGroups[groupIndex]->OpenList.clear();
				NodeGroups[groupIndex]->ClosedList.clear();
				NodeGroups[groupIndex]->Path.clear();;
				NodeGroups[groupIndex]->GoalState.Set({ 0, 0 });
				NodeGroups[groupIndex]->InUse = false;

			}
			attacker->AStar.NodeGroupIndex = -1;
			attacker->AStar.CurrentPathIndex = 0;

			continue;  //Path did not lead to tower

		}
		
		glm::vec2 Goal2DPosition= GameManager->GridToWorld(glm::vec2(path[currentGoal].Get()));
		glm::vec3 GoalPosition = glm::vec3(Goal2DPosition.x, 0, Goal2DPosition.y);
		distance = GoalPosition - attacker->Transform.Position;
		distance.y = 0;

		if (glm::length(distance) < 0.15f)
		{
			attacker->AStar.CurrentPathIndex++;
		}

		glm::vec3 dir = glm::normalize(distance);
		attacker->Transform.Position += dir * GameManager->GetAttackerSpeed() * deltaTime;

	}
}



void AttackerAgentSystem::FindPathToTower(AttackerEntity* attacker, TowerEntity* tower)
{
	attacker->Stats.GridPosition = GameManager->WorldToGrid(glm::vec2(attacker->Transform.Position.x, attacker->Transform.Position.z));


	NodeGroup* nodeGroup = nullptr;
	bool found = false;
	int nodeGroupIndex = -1;

	for (auto group : NodeGroups)
	{
		nodeGroupIndex++;
		if (!group->InUse)
		{
			nodeGroup = group;
			nodeGroup->InUse = true;
			found = true;
			break;
		}
	}

	if (!found)
	{
		nodeGroup = new NodeGroup;
		nodeGroup->ClosedList.reserve(GameManager->GetGridDimensions() * GameManager->GetGridDimensions());
		nodeGroup->OpenList.reserve(GameManager->GetGridDimensions() * GameManager->GetGridDimensions());
		nodeGroup->InUse = true;
		NodeGroups.push_back(nodeGroup);

		nodeGroupIndex = (int)NodeGroups.size() - 1;
	}

	attacker->AStar.NodeGroupIndex = nodeGroupIndex;

	AStar::AStarNode<AICourseAStar::State, float>* currentNode = new AStar::AStarNode<AICourseAStar::State, float>(nullptr, {0,0}, 0, 0, 0);
	currentNode->state[0] = attacker->Stats.GridPosition.x;
	currentNode->state[1] = attacker->Stats.GridPosition.y;
	currentNode->state.GridDimensions = GameManager->GetGridDimensions();

	nodeGroup->GoalState = AICourseAStar::State (tower->Stats.GridPosition.x, tower->Stats.GridPosition.y);
	nodeGroup->GoalState.GridDimensions = GameManager->GetGridDimensions();

	

	AStar::InitializePathFinding(nodeGroup->OpenList, nodeGroup->ClosedList, currentNode, nodeGroup->GoalState, 1.0f,
		AICourseAStar::Actions, GameManager->GetPathFindingMap(), AICourseAStar::CalculateGValue, AICourseAStar::CalculateHValue, AICourseAStar::IsValidAction);
		
	AStar::PathFindingState findingState;
	bool findingInProgress = true;
	int takenAction = -1;

	

	while (findingInProgress)
	{
		findingState = AStar::PathFindStep(nodeGroup->OpenList, nodeGroup->ClosedList, currentNode, nodeGroup->GoalState, 1.0f,
			AICourseAStar::Actions, takenAction, GameManager->GetPathFindingMap(), AICourseAStar::CalculateGValue, AICourseAStar::CalculateHValue, AICourseAStar::IsValidAction);

		if (findingState == AStar::PathFindingState::GoalCannotBeReached)
		{
			//Get the node with smallest fValue
			AStar::AStarNode<AICourseAStar::State, float>* closestNode = nodeGroup->ClosedList.front();
			for (auto node : nodeGroup->ClosedList)
			{
				if (node->fValue < closestNode->fValue)
				{
					closestNode = node;
				}
			}
			CalculatePathVector(attacker, closestNode);

			findingInProgress = false;
		}
		if (findingState == AStar::PathFindingState::GoalFound)
		{
			CalculatePathVector(attacker, currentNode);

			findingInProgress = false;
		}
	}
}

void AttackerAgentSystem::CalculatePathVector(AttackerEntity* attacker, AStar::AStarNode<AICourseAStar::State, float>* closestNode)
{
	NodeGroups[attacker->AStar.NodeGroupIndex]->Path.push_front(closestNode->state);
	while (closestNode->parent)
	{
		NodeGroups[attacker->AStar.NodeGroupIndex]->Path.push_front(closestNode->parent->state);
		closestNode = closestNode->parent;
	}
}


int AttackerAgentSystem::FindNearestEnemyTower(AttackerEntity* attacker)
{
	int nearestTowerID = -1;
	float minDistance = 0;
	int playerID = attacker->Player.PlayerIndex;

	auto* towerStorage = WorldEntityManager->GetCastedEntityStorage<TowerEntity>();
	for (int i = 0; i < towerStorage->GetEntityCount(); i++)
	{
		TowerEntity* tower = towerStorage->GetEntity(i);
		if (!tower)
		{
			continue;
		}
		if (tower->Player.PlayerIndex == playerID)
		{
			continue;
		}
		if (tower->Stats.Health <= 0)
		{
			continue;
		}
		if (nearestTowerID == -1)
		{
			nearestTowerID = tower->GetID();
			glm::vec3 diff = tower->Transform.Position - attacker->Transform.Position;
			diff.y = 0;
			minDistance = glm::length(diff);

		}
		glm::vec3 diff = tower->Transform.Position - attacker->Transform.Position;
		diff.y = 0;
		float distance = glm::length(diff);

		if (distance < minDistance)
		{
			nearestTowerID = tower->GetID();
			minDistance = distance;
		}
	}
	return nearestTowerID;
}

int AttackerAgentSystem::FindNearestOwnedTower(AttackerEntity* attacker)
{
	int nearestTowerID = -1;
	float minDistance = 0;
	int playerID = attacker->Player.PlayerIndex;

	auto* towerStorage = WorldEntityManager->GetCastedEntityStorage<TowerEntity>();
	for (int i = 0; i < towerStorage->GetEntityCount(); i++)
	{
		TowerEntity* tower = towerStorage->GetEntity(i);
		if (!tower)
		{
			continue;
		}
		if (tower->Player.PlayerIndex != playerID)
		{
			continue;
		}
		if (tower->Stats.Health <= 0)
		{
			continue;
		}
		if (nearestTowerID == -1)
		{
			nearestTowerID = tower->GetID();
			glm::vec3 diff = tower->Transform.Position - attacker->Transform.Position;
			diff.y = 0;
			minDistance = glm::length(diff);

		}
		glm::vec3 diff = tower->Transform.Position - attacker->Transform.Position;
		diff.y = 0;
		float distance = glm::length(diff);

		if (distance < minDistance)
		{
			nearestTowerID = tower->GetID();
			minDistance = distance;
		}
	}
	return nearestTowerID;
}