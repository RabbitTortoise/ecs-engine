#pragma once

#include <Engine/ECS/Component.h>
#include <glm/glm.hpp>			// Include glm

class CarryComponent : public Component
{
public:
	CarryComponent() : Component()
	{
		GoldCarried = 0.0f;

	}
	CarryComponent(const CarryComponent& c) :
		GoldCarried(c.GoldCarried),

		Component(static_cast<Component>(c)) 
	{}
	CarryComponent(CarryComponent&&c) noexcept :
		GoldCarried(std::move(c.GoldCarried)),

		Component(std::move(c)) 
	{}

	CarryComponent& operator=(const CarryComponent& c)
	{  
		GoldCarried = c.GoldCarried;

		return *this; 
	}

	float GoldCarried;

	inline static std::string ComponentName = "CarryComponent";
	inline static int ComponentIndex = -1;
};
