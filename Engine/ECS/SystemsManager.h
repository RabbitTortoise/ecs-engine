#pragma once

#include <string>
#include <vector>
#include <map>
#include <System.h>

class EntityManager;

class SystemsManager
{
public:
	SystemsManager(EntityManager* entityManager) 
		: WorldEntityManager(entityManager) {}

	

	/// <summary>
	/// Adds a new system to be updated each frame.
	/// </summary>
	/// <param name="system">Pointer to system to be added.</param>
	/// <returns>System's ID</returns>
	//int AddSystem(System* system, std::string systemName, System::SystemType systemType);

	/// <summary>
	/// Adds a new system to be updated each frame.
	/// </summary>
	/// <param name="system">Pointer to system to be added.</param>
	/// <returns>System's ID</returns>
	
	template <typename T>
	int AddSystem(T* system)
	{
		std::string systemName = T::SystemName;
		System::SystemType systemType = T::SystemType;

		//If system already exists, return
		auto it = SystemNames.find(systemName);
		if (it != SystemNames.end())
		{
			return it->second;
		}

		Systems.push_back(system);
		system->bEnabled = true;
		system->SetType(systemType);
		system->SetID(static_cast<int>(Systems.size()) - 1);
		SystemNames.insert(std::pair<std::string, int>(systemName, system->GetID()));

		return(system->GetID());
	}
	

	/// <summary>
	/// Disables the system with given ID.
	/// </summary>
	/// <param name="systemID">ID of the system</param>
	/// <returns>True if operation was successful</returns>
	bool DisableSystem(int systemID);

	/// <summary>
	/// Enables the system with given ID.
	/// </summary>
	/// <param name="systemID">ID of the system</param>
	/// <returns>True if operation was successful</returns>
	bool EnableSystem(int systemID);

	/// <summary>
	/// Updates all systems.
	/// </summary>
	/// <param name="deltaTime">Time between frames</param>
	void UpdateSystems(float deltaTime);

	/// <summary>
	/// Updates all render systems.
	/// </summary>
	/// <param name="deltaTime">Time between frames</param>
	void UpdateRenderSystems(float deltaTime);

	/// <summary>
	/// Updates all collision systems.
	/// </summary>
	/// <param name="deltaTime">Time between frames</param>
	void UpdateCollisionSystems(float deltaTime);

	System* GetSystemByName(std::string systemName);

	template <typename T>
	T* GetSystem()
	{
		std::string systemName = T::SystemName;
		int id = -1;
		auto it = SystemNames.find(systemName);
		if (it != SystemNames.end())
		{
			id = it->second;
			for (System* s : Systems)
			{
				if (s->GetID() == id)
				{
					return static_cast<T*>(s);
				}
			}
		}

		return nullptr;
	}

	
	void DestroySystems();
	void InitSystems();

protected:
	EntityManager* WorldEntityManager;
	std::vector<System*> Systems;

	/// <summary>
	/// First: System Name, Second: System ID
	/// </summary>
	std::map<std::string, int> SystemNames;
};

