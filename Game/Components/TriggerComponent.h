#pragma once

#include <Engine/ECS/Component.h>
#include <glm/glm.hpp>			// Include glm

class TriggerComponent : public Component
{
public:
	TriggerComponent() : Component()
	{
		CurrentAngle = 0.0f;
		MiddlePoint = glm::vec3();
		Speed = 0.0f;
		Radius = 0.0f;
	}
	TriggerComponent(const TriggerComponent& c) :
		CurrentAngle(c.CurrentAngle),
		MiddlePoint(c.MiddlePoint),
		Speed(c.Speed),
		Radius(c.Radius),
		Component(static_cast<Component>(c)) 
	{}
	TriggerComponent(TriggerComponent&&c) noexcept :
		CurrentAngle(std::move(c.CurrentAngle)),
		MiddlePoint(std::move(c.MiddlePoint)),
		Speed(std::move(c.Speed)),
		Radius(std::move(c.Radius)),
		Component(std::move(c)) 
	{}

	TriggerComponent& operator=(const TriggerComponent& c)
	{  
		CurrentAngle = c.CurrentAngle;
		MiddlePoint = c.MiddlePoint;
		Speed = c.Speed;
		Radius = c.Radius;
		return *this; 
	}

	float CurrentAngle;
	glm::vec3 MiddlePoint;
	float Speed;
	float Radius;

	inline static std::string ComponentName = "TriggerComponent";
	inline static int ComponentIndex = -1;
};
