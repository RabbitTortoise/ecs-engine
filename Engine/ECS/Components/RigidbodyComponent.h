#pragma once

#include <Engine/ECS/Component.h>
#include <glm/glm.hpp>			// Include glm
#include <string>

class RigidbodyComponent : public Component
{
public:
	RigidbodyComponent() : Component()
	{
		Gravity = glm::vec3();
		Force = glm::vec3();
		Velocity = glm::vec3();
		Mass = 0;
		AffectedByGravity = false;
		StaticFriction = 0;
		DynamicFriction = 0;
		Restitution = 0;
	}
	RigidbodyComponent(const RigidbodyComponent& c) :
		Gravity(c.Gravity),
		Force(c.Force),
		Velocity(c.Velocity),
		Mass(c.Mass),
		AffectedByGravity(c.AffectedByGravity),
		StaticFriction(c.StaticFriction),
		DynamicFriction(c.DynamicFriction),
		Restitution(c.Restitution),
		Component(static_cast<Component>(c)) 
	{}
	RigidbodyComponent(RigidbodyComponent&&c) noexcept :
		Gravity(std::move(c.Gravity)),
		Force(std::move(c.Force)),
		Velocity(std::move(c.Velocity)),
		Mass(std::move(c.Mass)),
		AffectedByGravity(std::move(c.AffectedByGravity)),
		StaticFriction(std::move(c.StaticFriction)),
		DynamicFriction(std::move(c.DynamicFriction)),
		Restitution(std::move(c.Restitution)),
		Component(std::move(c)) 
	{}

	RigidbodyComponent& operator=(const RigidbodyComponent& c)
	{  
		Gravity = c.Gravity;
		Force = c.Force;
		Velocity = c.Velocity;
		Mass = c.Mass;
		AffectedByGravity = c.AffectedByGravity;
		StaticFriction = c.StaticFriction;
		DynamicFriction = c.DynamicFriction;
		Restitution = c.Restitution;
		return *this; 
	}

	glm::vec3 Gravity;
	glm::vec3 Force;
	glm::vec3 Velocity;
	float Mass;
	bool AffectedByGravity;
	float StaticFriction;
	float DynamicFriction;
	float Restitution;


	inline static const std::string ComponentName = "RigidbodyComponent";
	inline static int ComponentIndex = -1;
};
