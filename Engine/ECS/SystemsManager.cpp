#include "Engine/ECS/SystemsManager.h"
#include "Engine/ECS/EntityManager.h"
#include "Engine/ECS/System.h"

/*
int SystemsManager::AddSystem(System* system, std::string systemName, System::SystemType systemType)
{
	Systems.push_back(system);
	system->bEnabled = true; 
	system->SetType(systemType);
	system->SetID(static_cast<int>(Systems.size()) - 1);
	SystemNames.insert(std::pair<std::string, int>(systemName, system->GetID()));

	return(system->GetID());
}
*/
bool SystemsManager::DisableSystem(int systemID)
{
	if (systemID < static_cast<int>(Systems.size()))
	{
		Systems.at(systemID)->bEnabled = false;
		return true;
	}
	return false;
}

bool SystemsManager::EnableSystem(int systemID)
{
	return false;
}

void SystemsManager::UpdateSystems(float deltaTime)
{
	//PreUpdate
	for (int i = 0; i < static_cast<int>(Systems.size()); i++)
	{
		if (Systems.at(i)->bEnabled && Systems.at(i)->GetType() == System::SystemType::System)
		{
			Systems.at(i)->PreUpdate(deltaTime);
		}
	}

	//Update
	for (int i = 0; i < static_cast<int>(Systems.size()); i++)
	{
		if (Systems.at(i)->bEnabled && Systems.at(i)->GetType() == System::SystemType::System)
		{
			Systems.at(i)->Update(deltaTime);
		}
	}

	//Update
	for (int i = 0; i < static_cast<int>(Systems.size()); i++)
	{
		if (Systems.at(i)->bEnabled && Systems.at(i)->GetType() == System::SystemType::System)
		{
			Systems.at(i)->PostUpdate(deltaTime);
		}
	}
}

void SystemsManager::UpdateRenderSystems(float deltaTime)
{
	//PreUpdate
	for (int i = 0; i < static_cast<int>(Systems.size()); i++)
	{
		if (Systems.at(i)->bEnabled && Systems.at(i)->GetType() == System::SystemType::RenderSystem)
		{
			Systems.at(i)->PreUpdate(deltaTime);
		}
	}
	//Update
	for (int i = 0; i < static_cast<int>(Systems.size()); i++)
	{
		if (Systems.at(i)->bEnabled && Systems.at(i)->GetType() == System::SystemType::RenderSystem)
		{
			Systems.at(i)->Update(deltaTime);
		}
	}

	//Update
	for (int i = 0; i < static_cast<int>(Systems.size()); i++)
	{
		if (Systems.at(i)->bEnabled && Systems.at(i)->GetType() == System::SystemType::RenderSystem)
		{
			Systems.at(i)->PostUpdate(deltaTime);
		}
	}
}

void SystemsManager::UpdateCollisionSystems(float deltaTime)
{
	//PreUpdate
	for (int i = 0; i < static_cast<int>(Systems.size()); i++)
	{
		if (Systems.at(i)->bEnabled && Systems.at(i)->GetType() == System::SystemType::CollisionSystem)
		{
			Systems.at(i)->PreUpdate(deltaTime);
		}
	}
	//Update
	for (int i = 0; i < static_cast<int>(Systems.size()); i++)
	{
		if (Systems.at(i)->bEnabled && Systems.at(i)->GetType() == System::SystemType::CollisionSystem)
		{
			Systems.at(i)->Update(deltaTime);
		}
	}

	//Update
	for (int i = 0; i < static_cast<int>(Systems.size()); i++)
	{
		if (Systems.at(i)->bEnabled && Systems.at(i)->GetType() == System::SystemType::CollisionSystem)
		{
			Systems.at(i)->PostUpdate(deltaTime);
		}
	}
}

System* SystemsManager::GetSystemByName(std::string systemName)
{
	int id = -1;
	auto it = SystemNames.find(systemName);
	if (it != SystemNames.end())
	{
		id = it->second;
		for (System* s : Systems)
		{
			if (s->GetID() == id)
			{
				return s;
			}
		}
	}

	return nullptr;
}

void SystemsManager::DestroySystems()
{
	for (int i = 0; i < static_cast<int>(Systems.size()); i++)
	{
		Systems.at(i)->Destroy();
	}
}

void SystemsManager::InitSystems()
{
	for (int i = 0; i < static_cast<int>(Systems.size()); i++)
	{
		if (!Systems.at(i)->GetInitialized())
		{
			Systems.at(i)->Init();
			Systems.at(i)->SetInitialized(true);
		}
	}
}


