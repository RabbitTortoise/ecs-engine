#pragma once

#include <Engine/ECS/Entity.h>
#include <Engine/ECS/EntityTypeStorage.h>
#include <string>
#include <vector>
#include <map>
#include <assert.h> 
#include <iostream>

/// <summary>
/// EntityManager holds all the different storage vectors for different types of entitys. 
/// It has template functions for adding new entity types to the system, adding and removing of entitys.
/// </summary>
class EntityManager
{
	//Allow other ECS system classes to access protected and private methods.
	friend class SystemsManager;
	friend class System;

public:
	EntityManager() {};
	~EntityManager() {};

	/// <summary>
	/// Return typeName as an integer.
	/// </summary>
	/// <param name="typeName">Container's TypeName</param>
	/// <returns>Container's type as int</returns>
	template <typename T>
	int GetEntityTypeId()
	{
		std::string typeName = T::GetEntityName();
		std::map<std::string, int>::iterator it = EntityTypeNames.find(typeName);
		if (it != EntityTypeNames.end())
		{
			return it->second;
		}
		return -1;
	}

	/// <summary>
	/// Return typeName as an integer.
	/// </summary>
	/// <param name="typeName">Container's TypeName</param>
	/// <returns>Container's type as int</returns>
	static int GetComponentTypeIdByName(std::string typeName)
	{
		std::map<std::string, int>::iterator it = ComponentTypeNames.find(typeName);
		if (it != ComponentTypeNames.end())
		{
			return it->second;
		}
		return -1;
	}

	/// <summary>
	/// Creates a new storage for given type of entities.
	/// Note that the given typeName is the identifier which can be used to get that type of entities later.
	/// </summary>
	/// <typeparam name="T">Entity type</typeparam>
	/// <param name="std::string used to find this container"></param>
	template <typename T>
	void CreateEntityTypeStorage()
	{
		std::string entityTypeName = T::GetEntityName();
		//If storage already exists, return
		auto it = EntityTypeNames.find(entityTypeName);
		if (it != EntityTypeNames.end())
		{
			return;
		}

		std::vector<std::string> componentTypeNames = T::GetComponentTypeNames();
		std::vector<int> componentTypes;
		for (auto t : componentTypeNames)
		{
			int typeId = GetComponentTypeIdByName(t);
			if (typeId == -1)
			{
				
				std::string error = "The following component was not initilized with CreateComponentType! (ID == -1): " + t;
				std::cout << error << std::endl;
				assert(0);
			}
			componentTypes.push_back(typeId);
		}

		EntityTypeNames.insert(std::pair<std::string, int>(entityTypeName, EntityTypeCount));
		EntityStorages.push_back(new EntityTypeStorage<T>(EntityTypeCount, componentTypes));
		EntityTypeCount++;
	}

	template <typename T>
	int CreateComponentType()
	{
		std::string componentTypeName = T::ComponentName;
		std::map<std::string, int>::iterator it = ComponentTypeNames.find(componentTypeName);
		if (it == ComponentTypeNames.end())
		{
			ComponentTypeNames.insert(std::pair<std::string,int>(componentTypeName, ComponentTypeCount));
			ComponentTypeCount++;

			T::ComponentIndex = ComponentTypeCount - 1;
			return ComponentTypeCount - 1;
		}

		return it->second;
	}



	void ResetAllStorages()
	{
		for (auto s : EntityStorages)
		{
			s->Reset();
		}
	}



	/// <summary>
	/// Adds a new Entity of given type
	/// </summary>
	/// <typeparam name="T">Entity type</typeparam>
	/// <param name="typeID">Container's type as int</param>
	/// <returns>Pointer to created entity</returns>
	template <typename T>
	T* CreateEntity(int entityTypeID)
	{
		for (int i = 0; i < EntityStorages.size(); i++)
		{
			if (EntityStorages.at(i)->GetEntityType() == entityTypeID)
			{
				//entity.Type = EntityStorages.at(i)->GetType();
				EntityTypeStorage<T>* s = static_cast<EntityTypeStorage<T>*>(EntityStorages.at(i));
				return s->CreateEntity();
			}
		}
		return nullptr;
	}

	/// <summary>
	/// Adds a new Entity of given type
	/// </summary>
	/// <typeparam name="T">Entity type</typeparam>
	/// <param name="typeID">Container's type as int</param>
	/// <returns>Pointer to created entity</returns>
	template <typename T>
	T* CreateEntity()
	{
		std::string entityTypeName = T::GetEntityName();
		int typeID = GetEntityTypeId<T>();
		for (int i = 0; i < EntityStorages.size(); i++)
		{
			if (EntityStorages.at(i)->GetEntityType() == typeID)
			{
				//entity.Type = EntityStorages.at(i)->GetType();
				EntityTypeStorage<T>* s = static_cast<EntityTypeStorage<T>*>(EntityStorages.at(i));
				return s->CreateEntity();
			}
		}
		return nullptr;
	}

	/// <summary>
	/// Gets a entity of given type and ID. Returns nullptr if Entity was not found.
	/// </summary>
	/// <typeparam name="T">Entity type</typeparam>
	/// <param name="id">ID of the Entity</param>
	/// <param name="typeID">Container's type as int</param>
	/// <returns>Pointer to the Entity if it exists</returns>
	template <typename T>
	T* GetEntity(int id, int entityTypeID)
	{
		//Console::Log("GetEntity, ID,Type: " + std::to_std::string(id) + "," + std::to_std::string(id));
		for (int i = 0; i < EntityStorages.size(); i++)
		{
			if (EntityStorages.at(i)->GetEntityType() == entityTypeID)
			{
				EntityTypeStorage<T>* s = static_cast<EntityTypeStorage<T>*>(EntityStorages.at(i));
				T* cT = static_cast<T*>(s->GetEntity(id));
				//Console::Log("Found Entity with ID: " + std::to_std::string(cT->GetID()));
				return cT;
			}
		}
		return nullptr;
	}

	


	template <typename T>
	std::vector<T>* GetEntityVector(int typeID)
	{
		for (int i = 0; i < EntityStorages.size(); i++)
		{
			if (EntityStorages.at(i)->GetEntityType() == typeID)
			{
				EntityTypeStorage<T>* s = static_cast<EntityTypeStorage<T>*>(EntityStorages.at(i));
				return s->GetEntityVector();
			}
		}
		return nullptr;
	}

	IEntityStorage* GetIEntityStorage(int typeID)
	{
		for (int i = 0; i < EntityStorages.size(); i++)
		{
			if (EntityStorages.at(i)->GetEntityType() == typeID)
			{
				return EntityStorages.at(i);
			}
		}
		return nullptr;
	}


	void MarkEntityForDeletion(int entityID, int EntityType)
	{
		EntitiesToDelete.push_back(std::make_pair(entityID, EntityType));
	}

	void DeleteMarkedEntities()
	{
		for (auto& e : EntitiesToDelete)
		{
			for (int i = 0; i < EntityStorages.size(); i++)
			{
				if (EntityStorages.at(i)->GetEntityType() == e.second)
				{
					bool success = EntityStorages.at(i)->RemoveEntity(e.first);
				}
			}
		}
		EntitiesToDelete.clear();
	}

	template <typename T>
	T* GetComponent(Entity* entity)
	{
		int componentID = GetComponentTypeIdByName(T::ComponentName);
		return entity->FetchComponent<T>(componentID);

	}

protected:


	/// <summary>
	/// Removes a entity with given ID and type. Note that because the entities are stored in a continuous array, the entity is only marked
	/// as disabled, and only overwritten after a new entity is added.
	/// </summary>
	/// <typeparam name="T">Entity type</typeparam>
	/// <param name="id">ID of the Entity</param>
	/// <param name="typeID">Container's type as int</param>
	/// <returns>True if removing was successful.</returns>
	template <typename T>
	bool RemoveEntity(int id, int entityTypeID)
	{
		for (int i = 0; i < EntityStorages.size(); i++)
		{
			if (EntityStorages.at(i)->GetEntityType() == entityTypeID)
			{
				EntityTypeStorage<T>* s = static_cast<EntityTypeStorage<T>*>(EntityStorages.at(i));
				return s->RemoveEntity(id);
			}
		}
		return false;
	}


	


private:
	//Storage varables.
	std::vector<IEntityStorage*> EntityStorages;
	std::map<std::string, int> EntityTypeNames;
	inline static std::map<std::string, int> ComponentTypeNames;
	int EntityTypeCount = 0;
	int ComponentTypeCount = 0;

	/// <summary>
	/// entityID, entityType
	/// </summary>
	std::vector<std::pair<int, int>> EntitiesToDelete;
};




























/*

/// <summary>
/// Contains all the entities in the ECS World and has functionality to add, get and remove Entities, to add, get and remove Comonents to/from Entities
/// and to query vectors of Entities based on different requirements.
/// </summary>
class EntityManager
{
public:
	EntityManager(EntityManager* entityManager) : WorldEntityManager(entityManager) {}

	/// <summary>
	/// Adds a new entity to the ECS World
	/// </summary>
	/// <returns>The created Entity</returns>
	Entity* AddEntity();

	/// <summary>
	/// Finds an entity with given ID
	/// </summary>
	/// <param name="entityID">ID of the Entity</param>
	/// <returns>Pointer to the entity if found.</returns>
	Entity* GetEntity(int entityID);

	/// <summary>
	/// Removes given entity from the world. Sets the given pointer to nullptr.
	/// </summary>
	/// <param name="entity">Pointer to the Entity to be removed</param>
	/// <returns>True if removing was successful</returns>
	bool RemoveEntity(Entity*& entity);

	/// <summary>
	/// Removes given entity from the world.
	/// </summary>
	/// <param name="entityID">ID of the entity.</param>
	/// <returns>True if removing was successful</returns>
	bool RemoveEntity(int entityID);

	/// <summary>
	/// Finds all the entities that meet the given requirements
	/// </summary>
	/// <param name="allowedTypes">Vector of allowed types</param>
	/// <returns>Entity ID:s that met the requirements</returns>
	std::vector<int> QueryEntitiesByType(std::vector<int> allowedTypes);

	/// <summary>
	/// Return typeName as an integer.
	/// </summary>
	/// <param name="typeName">Container's TypeName</param>
	/// <returns>Container's type as int</returns>
	int GetTypeIdByName(std::string typeName);

	/// <summary>
	/// Adds a new entity to an Entity. Returns nullptr if something goes wrong.
	/// </summary>
	/// <typeparam name="T">Entity type</typeparam>
	/// <param name="entityID">ID of the entity</param>
	/// <param name="entityTypeName">Entity's type as std::string</param>
	/// <returns>Pointer to added Entity</returns>
	template <typename T>
	T* AddEntityToEntity(int entityID, std::string entityTypeName)
	{
		int typeID = WorldEntityManager->GetTypeIdByName(entityTypeName);
		if (entityID < static_cast<int>(IndexUsage.size()))
		{
			if (IndexUsage.at(entityID))
			{
				T* c = WorldEntityManager->CreateEntity<T>(typeID);
				Entities.at(entityID)->GetEntitys().insert(std::pair<int, int>(c->GetID(), typeID));
				return c;
			}
		}
		return nullptr;
	}

	/// <summary>
	/// Gets a entity from an Entity. Returns nullptr if something goes wrong.
	/// </summary>
	/// <typeparam name="T">Entity type</typeparam>
	/// <param name="entityID">ID of the entity</param>
	/// <param name="entityTypeName">Entity's type as std::string</param>
	/// <returns>Pointer to Entity if found</returns>
	template <typename T>
	T* GetEntityFromEntity(int entityID, std::string entityTypeName)
	{
		int typeID = WorldEntityManager->GetTypeIdByName(entityTypeName);

		Entity* entity = GetEntity(entityID);
		if (entity != nullptr)
		{
			for (auto& c : entity->GetEntitys())
			{
				if (c.second == typeID)
				{				
					T* entity = WorldEntityManager->GetEntity<T>(c.first, typeID);
					if (entity != nullptr)
					{
						return entity;
					}
					break;
				}
			}
		}
		return nullptr;
	}

	/// <summary>
	/// Removes a entity from an Entity. Returns false if something goes wrong.
	/// </summary>
	/// <typeparam name="T">Entity type</typeparam>
	/// <param name="entityID">ID of the entity</param>
	/// <param name="entityTypeName">Entity's type as std::string</param>
	/// <returns>True if removing was successful</returns>
	template <typename T>
	bool RemoveEntityFromEntity(int entityID, std::string entityTypeName)
	{
		int typeID = WorldEntityManager->GetTypeIdByName(entityTypeName);
		Entity* entity = GetEntity(entityID);
		if (entity != nullptr)
		{
			for (auto& c : entity->GetEntitys())
			{
				if (c.second == typeID)
				{
					if (WorldEntityManager->RemoveEntity<T>(c.first, typeID))
					{
						//Console::Log("Removed Entity of type" + std::to_std::string(typeID) + " from Entity " + std::to_std::string(entity->GetID()));
						entity->GetEntitys().erase(c.first);
						return true;
					}
				}
			}
		}
		return false;
	}

	/// <summary>
	/// Removes a entity from an Entity. Returns false if something goes wrong.
	/// </summary>
	/// <typeparam name="T">Entity type</typeparam>
	/// <param name="entityID">ID of the entity</param>
	/// <param name="entityTypeName">Entity's type as std::string</param>
	/// <returns>True if removing was successful</returns>
	template <typename T>
	bool RemoveEntityFromEntity(int entityID, std::string entityTypeName, int entityID)
	{
		int typeID = WorldEntityManager->GetTypeIdByName(entityTypeName);
		Entity* entity = GetEntity(entityID);
		if (entity != nullptr)
		{
			for (auto& c : entity->GetEntitys())
			{
				if (c.first == entityID && c.second == typeID)
				{
					if (WorldEntityManager->RemoveEntity<T>(entityID, typeID))
					{
						//Console::Log("Removed Entity of type" + std::to_std::string(typeID) + " from Entity " + std::to_std::string(entity->GetID()));
						entity->GetEntitys().erase(c.first);
						return true;
					}
				}
			}
		}
		return false;
	}

	void ClearEntities();
	
private:

	EntityManager* WorldEntityManager;

	std::vector<Entity*> Entities;
	std::vector<bool> IndexUsage;
};

*/