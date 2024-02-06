#pragma once

#include <Engine/ECS/Component.h>
#include <glm/glm.hpp>			// Include glm


//#define COMPONENTCONSTRUCT(CLASS, a, b) CLASS(const CLASS& component) : a(component.a), b(component.b), Component(static_cast<Component>(component)){}


class PlayerComponent : public Component
{
public:
	PlayerComponent() : Component()
	{
		PlayerIndex = -1;
		IsAI = false;

	}
	PlayerComponent(const PlayerComponent& c) :
		PlayerIndex(c.PlayerIndex),
		IsAI(c.IsAI),

		Component(static_cast<Component>(c)) 
	{}
	PlayerComponent(PlayerComponent&&c) noexcept :
		PlayerIndex(std::move(c.PlayerIndex)),
		IsAI(std::move(c.IsAI)),

		Component(std::move(c)) 
	{}


	PlayerComponent& operator=(const PlayerComponent& c)
	{  
		PlayerIndex = c.PlayerIndex;
		IsAI = c.IsAI;

		return *this; 
	}

	int PlayerIndex;
	bool IsAI;

	inline static std::string ComponentName = "PlayerComponent";
	inline static int ComponentIndex = -1;
};
