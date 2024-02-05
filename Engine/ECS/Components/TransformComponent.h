#pragma once

#include <Engine/ECS/Component.h>
#include <glm/glm.hpp>			// Include glm
#include <glm/gtc/quaternion.hpp>

class TransformComponent : public Component
{
public:
	TransformComponent() : Component()
	{
		Position = glm::vec3();
		EulerAngles = glm::vec3();
		Orientation = glm::quat();
		Scale = glm::vec3();
		UseQuaternion = false;
	}
	TransformComponent(const TransformComponent& c) :
		Position(c.Position),
		EulerAngles(c.EulerAngles),
		Orientation(c.Orientation),
		Scale(c.Scale),
		UseQuaternion(c.UseQuaternion),
		Component(static_cast<Component>(c)) 
	{}
	TransformComponent(TransformComponent&&c) noexcept : 
		Position(std::move(c.Position)),
		EulerAngles(std::move(c.EulerAngles)),
		Orientation(std::move(c.Orientation)),
		Scale(std::move(c.Scale)),
		UseQuaternion(std::move(c.UseQuaternion)),
		Component(std::move(c)) 
	{}

	TransformComponent& operator=(const TransformComponent& c) 
	{
		Position = c.Position;
		EulerAngles = c.EulerAngles;
		Orientation = c.Orientation;
		Scale = c.Scale;
		UseQuaternion = c.UseQuaternion;
		return *this; 
	}

	glm::vec3 Position;
	glm::vec3 EulerAngles;
	glm::quat Orientation;
	glm::vec3 Scale;
	bool UseQuaternion;

	inline static const std::string ComponentName = "TransformComponent";
	inline static int ComponentIndex = -1;
};
