#pragma once

#include <Engine/ECS/EntityManager.h>
#include <Engine/ECS/SystemsManager.h>


namespace IECS
{
	void Init();
	void Destroy();
	void ResetECSWorld();

	EntityManager* GetEntityManager();
	SystemsManager* GetSystemsManager();

}