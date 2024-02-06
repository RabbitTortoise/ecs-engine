#pragma once

#include <Engine/ECS/System.h>
#include <Engine/ECS/Components/TransformComponent.h>
#include <Game/Components/GameStatComponent.h>
#include <Game/Components/PlayerComponent.h>
#include <Game/Components/CarryComponent.h>
#include <iostream>
#include <Game/Scripts/AICourseGameManager.h>
#include <Game/Entities/TowerEntity.h>
#include <Game/Entities/GoldEntity.h>
#include <Game/Entities/CollectorEntity.h>


class CollectorAgentSystem : public IComponentTypeQuerySystem
{
public:
	CollectorAgentSystem(EntityManager* entityManager, AICourseGameManager* gameManager) : IComponentTypeQuerySystem(entityManager)
	{
		GameManager = gameManager;
		WorldEntityManager = entityManager;
	}

	void Init()
	{
	}

	void Destroy()
	{
	}

	void PreUpdate(float deltaTime)
	{
	}


	void ComponentTypeQueryUpdate(float deltaTime, IEntityStorage* entityTypeStorage)
	{
		
		auto* entityStorage = WorldEntityManager->CastEntityStorage<CollectorEntity>(entityTypeStorage);

		for (int i = 0; i < entityStorage->GetEntityCount(); i++)
		{
			CollectorEntity* collector = entityStorage->GetEntity(i);
			if (!collector)
			{
				continue;
			}

			
				
			if (collector->Carry.GoldCarried == 0)
			{
				
				//Get target gold
				if (collector->Stats.TargetEntity == -1)
				{
					collector->Stats.TargetEntity = FindNearestGold(collector);
					if (collector->Stats.TargetEntity == -1)
					{
						continue; //No Gold Found
					}
				}
				GoldEntity* targetGold = WorldEntityManager->GetEntity<GoldEntity>(collector->Stats.TargetEntity);
				if (!targetGold)
				{
					collector->Stats.TargetEntity = -1;
					continue; //Gold target no longer valid;
				}
				

				//Check if close to target
				glm::vec3 distance = targetGold->Transform.Position - collector->Transform.Position;
				distance.y = 0;
				float carryCapacity = GameManager->GetCollectorCarryCapacity();
				
				if (glm::length(distance) < GameManager->GetCollectorInterractRadius())
				{
					if (targetGold->Stats.Health <= 0)
					{
						collector->Stats.TargetEntity = -1;
						continue;
					}
					if (targetGold->Stats.Health < carryCapacity)
					{
						collector->Carry.GoldCarried = targetGold->Stats.Health;
						targetGold->Stats.Health = 0;

					}
					else
					{
						collector->Carry.GoldCarried = carryCapacity;
						targetGold->Stats.Health -= carryCapacity;
					}

					if (targetGold->Stats.Health <= 0)
					{
						GameManager->RemoveGold(targetGold->Stats.GridPosition, targetGold->GetID());
					}

					collector->Stats.TargetEntity = -1;
					continue;
				}

				//Go to target gold
				glm::vec3 dir = glm::normalize(distance);
				collector->Transform.Position += dir * GameManager->GetCollectorSpeed() * deltaTime;

			}
			else 
			{
				if (collector->Stats.TargetEntity == -1)
				{
					collector->Stats.TargetEntity = FindNearestTower(collector);
					if (collector->Stats.TargetEntity == -1)
					{
						continue; //No Tower Found
					}

				}
				
				TowerEntity* targetTower = WorldEntityManager->GetEntity<TowerEntity>(collector->Stats.TargetEntity);
				if (!targetTower)
				{
					collector->Stats.TargetEntity = -1;
					continue; //Tower target no longer valid;
				}

				//Check if close to target
				glm::vec3 distance = targetTower->Transform.Position - collector->Transform.Position;
				distance.y = 0;

				if (glm::length(distance) < GameManager->GetCollectorInterractRadius())
				{
					if (targetTower->Stats.Health <= 0)
					{
						collector->Stats.TargetEntity = -1;
						continue;
					}

					
					GameManager->AddGoldToPlayer(collector->Player.PlayerIndex, (int)collector->Carry.GoldCarried);
					collector->Carry.GoldCarried = 0;
					collector->Stats.TargetEntity = -1;
					continue;
				}

				//Go to target tower
				glm::vec3 dir = glm::normalize(distance);
				collector->Transform.Position += dir * GameManager->GetCollectorSpeed() * deltaTime;
			}	
		}
	}

	int FindNearestGold(CollectorEntity* collector)
	{
		int nearestGoldID = -1;
		float minDistance = 0;
		int playerID = collector->Player.PlayerIndex;

		auto* goldStorage = WorldEntityManager->GetCastedEntityStorage<GoldEntity>();
		for (int i = 0; i < goldStorage->GetEntityCount(); i++)
		{
			GoldEntity* gold = goldStorage->GetEntity(i);
			if (!gold)
			{
				continue;
			}
			if (gold->Stats.Health <= 0)
			{
				continue;
			}
			if (nearestGoldID == -1)
			{
				nearestGoldID = gold->GetID();
				glm::vec3 diff = gold->Transform.Position - collector->Transform.Position;
				diff.y = 0;
				minDistance = glm::length(diff);

			}
			glm::vec3 diff = gold->Transform.Position - collector->Transform.Position;
			diff.y = 0;
			float distance = glm::length(diff);
			if (distance < minDistance)
			{
				nearestGoldID = gold->GetID();
				minDistance = distance;
			}
		}
		return nearestGoldID;
	}

	int FindNearestTower(CollectorEntity* collector)
	{
		int nearestTowerID = -1;
		float minDistance = 0;
		int playerID = collector->Player.PlayerIndex;

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
				glm::vec3 diff = tower->Transform.Position - collector->Transform.Position;
				diff.y = 0;
				minDistance = glm::length(diff);

			}
			glm::vec3 diff = tower->Transform.Position - collector->Transform.Position;
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


	

	float RandomNumber0_1()
	{
		return (static_cast <float> (rand()) / static_cast <float> (RAND_MAX));
	}


	inline static const std::string SystemName = "CollectorAgentSystem";
	inline static const System::SystemType SystemType = System::SystemType::System;

private:
	AICourseGameManager* GameManager;
	EntityManager* WorldEntityManager;
};
