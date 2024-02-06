#pragma once

#include <Engine/ECS/Component.h>
#include <glm/glm.hpp>			// Include glm

class GameStatComponent : public Component
{
public:
	GameStatComponent() : Component()
	{
		Health = 0.0f;
		TargetEntity = -1;
		TargetGridPosition = glm::ivec2(0, 0);
		GridPosition = glm::ivec2(0,0);

	}
	GameStatComponent(const GameStatComponent& c) :
		Health(c.Health),
		TargetEntity(c.TargetEntity),
		TargetGridPosition(c.TargetGridPosition),
		GridPosition(c.GridPosition),

		Component(static_cast<Component>(c)) 
	{}
	GameStatComponent(GameStatComponent&&c) noexcept :
		Health(std::move(c.Health)),
		TargetEntity(std::move(c.TargetEntity)),
		TargetGridPosition(std::move(c.TargetGridPosition)),
		GridPosition(std::move(c.GridPosition)),

		Component(std::move(c)) 
	{}

	GameStatComponent& operator=(const GameStatComponent& c)
	{  
		Health = c.Health;
		TargetEntity = c.TargetEntity;
		TargetGridPosition = c.TargetGridPosition;
		GridPosition = c.GridPosition;

		return *this; 
	}

	float Health;
	int TargetEntity;
	glm::ivec2 TargetGridPosition;
	glm::ivec2 GridPosition;

	inline static std::string ComponentName = "GameStatComponent";
	inline static int ComponentIndex = -1;
};
