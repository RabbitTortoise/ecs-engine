#pragma once

#include <vector>
#include "Engine/ECS/EntityManager.h"

class System
{
	friend class SystemsManager;
	friend class EntityManager;


public:
	enum class SystemType { System = 0, RenderSystem, CollisionSystem };

	System(EntityManager* entityManager) : WorldEntityManager(entityManager) { bEnabled = false; bInitialized = false; }
	~System() {}

	virtual void Init() = 0;
	virtual void PreUpdate(float deltaTime) = 0;
	virtual void Update(float deltaTime) = 0;
	virtual void PostUpdate(float deltatime) = 0;
	virtual void Destroy() = 0;

	/// <summary>
	/// Set System to be enabled or disabled.
	/// </summary>
	/// <param name="enabled">True/False</param>
	void SetEnabled(bool enabled) { bEnabled = enabled; }
	void SetInitialized(bool intialized) { bInitialized = intialized; }

	int GetID() { return ID; }
	SystemType GetType() { return Type; }
	bool GetInitialized() { return bInitialized; }

protected:

	void SetID(int id) { ID = id; }
	void SetType(SystemType type) { Type = type; }

	EntityManager* WorldEntityManager;

private:
	int ID = -1;
	SystemType Type = SystemType::System;
	bool bEnabled;
	bool bInitialized;
};

class IComponentTypeQuerySystem : public System
{
public:
	IComponentTypeQuerySystem(EntityManager* entityManager) : System(entityManager) {}

	template <typename T>
	void AddRequiredComponentType() 
	{ 
		int componentType = WorldEntityManager->GetComponentTypeIdByName(T::ComponentName);
		//If requirement already exists, return

		

		auto it = std::find(RequiredComponentTypes.begin(), RequiredComponentTypes.end(), componentType);
		if (it != RequiredComponentTypes.end())
		{
			return;
		}

		RequiredComponentTypes.push_back(componentType); 
	};
	void AddForbiddenComponentType(int componentType) { ForbiddenComponentTypes.push_back(componentType); };

	template <typename T>
	bool AddEntityTypeIfCompatible()
	{
		std::vector<std::string> componentTypes = T::GetComponentTypeNames();
		int entityType = WorldEntityManager->GetEntityTypeId<T>();
		return AddEntityTypeIfCompatible(entityType, componentTypes);
	}

	virtual void Init() = 0;
	virtual void Destroy() = 0;
	virtual void PreUpdate(float deltaTime) {};
	
	void Update(float deltaTime)
	{
		for (auto et : CompatibleEntityTypes)
		{
			ComponentTypeQueryUpdate(deltaTime, WorldEntityManager->GetIEntityStorage(et));
		}
	};
	virtual void PostUpdate(float deltaTime) {};
	virtual void ComponentTypeQueryUpdate(float deltaTime, IEntityStorage* entityTypeStorage) = 0;

protected:
	std::vector<int> RequiredComponentTypes;
	std::vector<int> ForbiddenComponentTypes;
	std::vector<int> CompatibleEntityTypes;

	virtual bool AddEntityTypeIfCompatible(int entityType, std::vector<std::string> componentTypes)
	{
		for (auto c : componentTypes)
		{
			int ct = WorldEntityManager->GetComponentTypeIdByName(c);
			for (auto fct : ForbiddenComponentTypes)
			{
				if (ct == fct)
				{
					return false;
				}
			}
			for (auto rct : RequiredComponentTypes)
			{
				if (ct == rct)
				{
					continue;
				}
			}
		}

		//If compatible entity already exists, return

		if (std::find(CompatibleEntityTypes.begin(), CompatibleEntityTypes.end(), entityType) != CompatibleEntityTypes.end())
		{
			return true;
		}

		CompatibleEntityTypes.push_back(entityType);
		return true;
	}

private:

};


template <class T>
class IEntityVectorQuerySystem : public System
{
public:
	IEntityVectorQuerySystem(EntityManager* entityManager, std::string entityTypeName) : System(entityManager)
	{
		EntityType = entityTypeName;
		EntityTypeID = entityManager->GetEntityTypeId<T>();
	}

	virtual void Init() = 0;
	virtual void Destroy() = 0;

	virtual void PreUpdate(float deltaTime) {};
	virtual void Update(float deltaTime)
	{
		EntityVectorQueryUpdate(deltaTime, GetEntityVector<T>(EntityTypeID));
	}
	virtual void PostUpdate(float deltaTime) {};
	virtual void EntityVectorQueryUpdate(float deltaTime, std::vector<T>* entities) = 0;

protected:
	std::vector<T>* GetEntityVector()
	{
		return WorldEntityManager->GetEntityVector<T>(EntityTypeID);
	}

private:
	std::string EntityType;
	int EntityTypeID;
};



/* Example System
#pragma once


#include "ECS/SystemsManager.h"
#include "Components/PositionComponent.h"

class MovementSystem : public IEntityQuerySystem
{
public:
	MovementSystem(EntityManager* entityManager) : IEntityQuerySystem(entityManager)
	{
		Console::Log("MovementSystem Constructor");
	}

	void Update(float deltaTime)
	{
		EntityQueryUpdate(deltaTime, WorldEntityManager->QueryEntitiesByType(TypesWanted));
	}


	void EntityQueryUpdate(float deltaTime, std::vector<int> entities)
	{
		if (!Done)
		{
			Done = true;
			Console::Log("MovementSystem: Entity count: " + std::to_std::string(entities.size()));
		}
		for (auto e : entities)
		{
			//Console::Log("Processing entity " + std::to_std::string(WorldEntityManager.GetEntity(e)->GetID()));
			PositionComponent* pos = WorldEntityManager->GetComponentFromEntity<PositionComponent>(WorldEntityManager->GetEntity(e)->GetID(), "PositionComponent");
			if (pos != nullptr)
			{
				pos->X += 10.0f * deltaTime;
				pos->Y += 10.0f * deltaTime;
				pos->Z += 10.0f * deltaTime;

				//Console::Log(std::to_std::string(WorldEntityManager.GetEntity(e)->GetID()) + "," + std::to_std::string(pos->GetID()) + "," + std::to_std::string(pos->Z) + "," + std::to_std::string(pos->Y) + "," + std::to_std::string(pos->Z));
			}
			else
			{
				Console::Log("Component NOT FOUND!");
			}
		}

	}


private:
	bool Done = false;
};
*/