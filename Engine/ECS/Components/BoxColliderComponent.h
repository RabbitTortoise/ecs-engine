#pragma once

#include <Engine/ECS/Component.h>
#include <glm/glm.hpp>			// Include glm
#include <string>

class BoxColliderComponent : public Component
{
public:
	BoxColliderComponent() : Component()
	{
		BoxHalfSize = glm::vec3();
		AxisX = glm::vec3();
		AxisY = glm::vec3();
		AxisZ = glm::vec3();
	}
	BoxColliderComponent(const BoxColliderComponent& c) :
		BoxHalfSize(c.BoxHalfSize),
		AxisX(c.AxisX),
		AxisY(c.AxisY),
		AxisZ(c.AxisZ),
		Component(static_cast<Component>(c)) 
	{}
	BoxColliderComponent(BoxColliderComponent&&c) noexcept :
		BoxHalfSize(std::move(c.BoxHalfSize)),
		AxisX(std::move(c.AxisX)),
		AxisY(std::move(c.AxisY)),
		AxisZ(std::move(c.AxisZ)),
		Component(std::move(c)) 
	{}

	BoxColliderComponent& operator=(const BoxColliderComponent& c)
	{  
		BoxHalfSize = c.BoxHalfSize;
		AxisX = c.AxisX;
		AxisY = c.AxisY;
		AxisZ = c.AxisZ;
		return *this; 
	}

	glm::vec3 BoxHalfSize;
	glm::vec3 AxisX;
	glm::vec3 AxisY;
	glm::vec3 AxisZ;

	inline static const std::string ComponentName = "BoxColliderComponent";
	inline static int ComponentIndex = -1;
};
