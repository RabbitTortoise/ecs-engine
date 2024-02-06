#pragma once

#include <Engine/ECS/Component.h>
#include <glm/glm.hpp>			// Include glm
#include <vector>
#include <Game/Scripts/Astar.h>

class AStarComponent : public Component
{
public:
	AStarComponent() : Component()
	{
		NodeGroupIndex = -1;
		CurrentPathIndex = 0;


	}
	AStarComponent(const AStarComponent& c) :
		NodeGroupIndex(c.NodeGroupIndex),
		CurrentPathIndex(c.CurrentPathIndex),

		Component(static_cast<Component>(c)) 
	{}
	AStarComponent(AStarComponent&&c) noexcept :
		NodeGroupIndex(std::move(c.NodeGroupIndex)),
		CurrentPathIndex(std::move(c.CurrentPathIndex)),

		Component(std::move(c)) 
	{}

	AStarComponent& operator=(const AStarComponent& c)
	{  
		NodeGroupIndex = c.NodeGroupIndex;
		CurrentPathIndex = c.CurrentPathIndex;

		return *this; 
	}

	int NodeGroupIndex;
	int CurrentPathIndex;

	inline static std::string ComponentName = "AStarComponent";
	inline static int ComponentIndex = -1;
};
