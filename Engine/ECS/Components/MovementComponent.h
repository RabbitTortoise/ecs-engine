#pragma once

#include <Engine/ECS/Component.h>
#include <glm/glm.hpp>			// Include glm

class MovementComponent : public Component
{
public:
	MovementComponent() : Component()
	{
		Direction = glm::vec3();
		Speed = 0.0f;
	}
	MovementComponent(const MovementComponent& c) :
		Direction(c.Direction),
		Speed(c.Speed),
		Component(static_cast<Component>(c)) 
	{}
	MovementComponent(MovementComponent&&c) noexcept :
		Direction(std::move(c.Direction)),
		Speed(std::move(c.Speed)),
		Component(std::move(c)) 
	{}

	MovementComponent& operator=(const MovementComponent& c) 
	{  
		Direction = c.Direction;
		Speed = c.Speed;
		return *this; 
	}

	glm::vec3 Direction;
	float Speed;

	inline static const std::string ComponentName = "MovementComponent";
	inline static int ComponentIndex = -1;
};
