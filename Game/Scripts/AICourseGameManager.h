#pragma once
#include <Engine/Core/ScriptingCore.h>
#include <vector>
#include <glm/glm.hpp>
#include <chrono>

#include <Engine/OpenGL/ECS/Components/MaterialComponent.h>
#include <Game/Scripts/Astar.h>

class EntityManager;
class SystemsManager;
class MeshComponent;

class MeshLoaderSystem;
class TextureLoaderSystem;
class ShaderLoaderSystem;
class CameraControllerSystem;


class AICourseGameManager
{

public:
	AICourseGameManager();
	~AICourseGameManager();

	struct AIPreferences
	{
		float MaxGoldDistance = 6.0f;

		float TowerPreference = 1.0f;
		float CollectorPreference = 1.0f;
		float AttackerPreference = 1.0f;

		float TowerAmountMultiplier = 1.0f;
		float CollectorMultiplier = 1.0f;
		float AttackerMultiplier = 1.0f;
	};

	struct PlayerStats
	{
		int Gold;
		int MainTowerID;
		std::vector<glm::ivec2> towerPositions;
	};

	void SetupGame();
	void SetupAI();
	void UpdateGame(float deltaTime);
	void CheckForGoldSpawn(float deltaTime);
	void UpdateAI(float deltaTime);
	void CreateBaseGrid(int gridDimension);
	

	bool SpawnGridBlock(int gridX, int gridY);
	void SpawnGold(int gridX, int gridY);
	void SpawnCollector(int gridX, int gridY, int playerIndex);
	void SpawnAttacker(int gridX, int gridY, int playerIndex);
	int FindNearestOwnedTower(glm::ivec2 gridPosition, int playerID);
	int FindNearestGold(glm::ivec2 gridPosition, float& distanceOut);
	int SpawnTower(int gridX, int gridY, int playerIndex, bool ignoreLimit = false);

	int FriendlyTowerCount(int playerID);
	int FriendlyCollectorCount(int playerID);
	int FriendlyAttackerCount(int playerID);


	void RemoveGold(glm::vec2 gridPos, int entityID);
	void RemoveTower(glm::vec2 gridPos, int entityID);
	void RemoveCollector(int entityID);
	void RemoveAttacker(int entityID);

	auto* GetPathFindingMap() { return &PathFindingMap; }

	int GetGridDimensions() { return GridDimensions; }
	float GetGridBlockDimensions() { return GridBlockDimensions; }

	float GetCollectorSpeed() { return CollectorSpeed; }
	float GetCollectorInterractRadius() { return CollectorInterractRadius; }
	float GetCollectorCarryCapacity() { return CollectorCarryCapacity; }
	
	float GetAttackerSpeed() { return AttackerSpeed; }
	float GetAttackerInterractRadius() { return AttackerInterractRadius; }

	glm::vec2 WorldToGrid(glm::vec2 world)
	{
		return glm::vec2(std::floor(GridDimensions / 2.0f  + world.x), std::floor(GridDimensions / 2.0f + world.y));
	}
	glm::vec2 GridToWorld(glm::vec2 grid)
	{
		float x = GridDimensions / -2.0f + grid.x;
		float y = GridDimensions / -2.0f + grid.y;
		return glm::vec2(x,y);
	}

	void AddGoldToPlayer(int playerIndex, int amount);

private:
	EntityManager* WorldEntityManager;
	SystemsManager* WorldSystemsManager;

	MeshLoaderSystem* MeshLoader;
	TextureLoaderSystem* TextureLoader;
	ShaderLoaderSystem* ShaderLoader;
	CameraControllerSystem* CameraController;



	std::vector<std::vector<int>> BaseGrid;
	std::vector<std::vector<bool>> PathFindingMap;
	std::vector<std::vector<bool>> PlacementMap;


	int GridDimensions = 32;
	float GridBlockDimensions = 0.5;
	


	int HumanPlayerIndex = 0;
	int AIPlayerIndex = 1;

	std::vector<PlayerStats> GamePlayers;

	int TowerGoldCost = 50;
	int AttackerGoldCost = 25;
	int CollectorGoldCost = 50;
	int StartingGold = 150;

	int TowerHealth = 100;
	float TowerMaxPlacementDistance = 7.0f;

	float CollectorSpeed = 3.0f;
	float CollectorInterractRadius = 0.5f;
	float CollectorCarryCapacity = 10.0f;
	int CollectorHealth = 20;

	float AttackerSpeed = 6.0f;
	int AttackerHealth = 20;
	float AttackerInterractRadius = 0.35f; 


	AIPreferences AIModifiers;
	float AITickCount = 0.0f;
	float NextAITick = 0.0f;
	float AITickDelay = 0.2f;

	float GoldSpawnTickCount = 0.0f;
	float NextGoldSpawnTick = 0.0f;
	float GoldSpawnTickDelay = 1.0f;

	float RandomNumber0_1();

	MaterialComponent HumanPlayerMaterial;
	MaterialComponent AIPlayerMaterial;

	
};