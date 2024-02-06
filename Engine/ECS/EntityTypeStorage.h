#pragma once

#include <vector>
#include <Engine/ECS/Component.h>
#include <Engine/ECS/Entity.h>


/// <summary>
/// This is used in storing all the entities in different vectors.
/// </summary>
class IEntityStorage
{
public:
	IEntityStorage(int entityType, std::vector<int> componentTypes) : EntityType(entityType), ComponentTypes(componentTypes) {}

	int GetEntityType() { return EntityType; }
	int GetEnabledEntitiesCount() { return EnabledEntitiesCount; }
	std::vector<int> GetComponentTypes() { return ComponentTypes; }


	virtual void Reset() = 0;

	virtual Entity* GetEntity(int id) = 0;
	virtual bool RemoveEntity(int id) = 0;
	virtual int GetEntityCount() = 0;

protected:
	int EntityType;
	int EnabledEntitiesCount = 0;
	std::vector<int> ComponentTypes;
};

/// <summary>
/// This is used to get to store one type of entity. It has methods for adding, removing and getting a single componet.
/// </summary>
/// <typeparam name="T"></typeparam>
template <class T>
class EntityTypeStorage : public IEntityStorage
{
public:

	EntityTypeStorage(int entityType, std::vector<int> componentTypes) : IEntityStorage(entityType, componentTypes)
	{
		Entities = new std::vector<T>;
		IndexUsage = new std::vector<bool>;
	}

	~EntityTypeStorage()
	{
		delete Entities;
		delete IndexUsage;
	}

	//Entity* GetEntity(int id)
	//{
	//	if (id < static_cast<int>(Entities->size()))
	//	{
	//		if (IndexUsage->at(id))
	//		{
	//			if (!Entities->at(id).GetEnabled())
	//			{
	//				return &Entities->at(id);
	//			}
	//		}
	//	}
	//	return nullptr;
	//}

	T* GetEntity(int id)
	{
		if (id < static_cast<int>(Entities->size()))
		{
			if (IndexUsage->at(id))
			{
				if (!Entities->at(id).GetEnabled())
				{
					return &Entities->at(id);
				}
			}
		}
		return nullptr;
	}


	/// <summary>
	/// Creates a new entity to the vector.
	/// </summary>
	/// <returns>Pointer of the added entity</returns>
	T* CreateEntity()
	{
		int usedIndex = -1;
		for (int i = 0; i < static_cast<int>(IndexUsage->size()); i++)
		{
			if (!(IndexUsage->at(i)))
			{
				usedIndex = i;
				break;
			}
		}
		if (usedIndex == -1)
		{
			usedIndex = static_cast<int>(IndexUsage->size());
			Entities->emplace_back();
			IndexUsage->push_back(true);
		}
		else
		{
			Entities->at(usedIndex) = T();
			IndexUsage->at(usedIndex) = true;
		}
		Entities->at(usedIndex).SetID(usedIndex);
		Entities->at(usedIndex).SetType(EntityType);
		EnabledEntitiesCount++;

		//Console::Log("Added Entity of type "+ std::to_std::string(Type) + " To ID: " + std::to_std::string(usedIndex));

		return &Entities->at(usedIndex);
	}

	/* DISABLED FOR NOW
	/// <summary>
	/// Gets a specific entity based on given ID.
	/// </summary>
	/// <param name="id">ID of the Entity</param>
	/// <returns>Entity of given ID, nullptr if the entity of that ID was not found or if the entity was disabled.</returns>
	T* FetchEntity(int id)
	{
		if (id < static_cast<int>(Entities->size()))
		{
			if (IndexUsage->at(id))
			{
				if (!Entities->at(id).GetEnabled())
				{
					return &Entities->at(id);
				}
			}
		}
		return nullptr;
	}
	*/

	/// <summary>
	/// Removes a entity with given ID. Note that because the entitys are stored in a continuous array, the entity is only marked
	/// as disabled, and only overwritten after a new entity is added.
	/// </summary>
	/// <param name="id">ID of the entity</param>
	/// <returns>Boolean based on if the entity was removed successfully.</returns>
	bool RemoveEntity(int id)
	{
		if (id < static_cast<int>(Entities->size()))
		{
			if (IndexUsage->at(id))
			{
				IndexUsage->at(id) = false;
				EnabledEntitiesCount--;
				return true;
			}
		}
		return false;
	}

	/// <summary>
	/// Returns a pointer to the entity container
	/// </summary>
	/// <returns>Component vector</returns>
	std::vector<T, std::allocator<T>>* GetEntityVector()
	{
		return Entities;
	}

	int GetEntityCount()
	{
		return static_cast<int>(Entities->size());
	}

	void Reset()
	{
		Entities->clear();
		IndexUsage->clear();
		EnabledEntitiesCount = 0;
	}

private:
	std::vector<T, std::allocator<T>>* Entities;
	std::vector<bool>* IndexUsage;
};


