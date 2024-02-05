#pragma once

#include <Engine/ECS/Component.h>
#include <glm/glm.hpp>			// Include glm

class CameraComponent : public Component
{
public:
	CameraComponent() : Component()
	{
		CameraID = 0;
		ProjectionMatrix = glm::mat4();
		Orientation = glm::mat4();
		Front = glm::vec3();
		Right = glm::vec3();
		Up = glm::vec3();
		Fov = 0;
		NearPlane = 0;
		FarPlane = 0;
	}
	CameraComponent(const CameraComponent& c) :
		CameraID(c.CameraID),
		ProjectionMatrix(c.ProjectionMatrix),
		Orientation(c.Orientation),
		Front(c.Front),
		Right(c.Right),
		Up(c.Up),
		Fov(c.Fov),
		NearPlane(c.NearPlane),
		FarPlane(c.FarPlane),
		Component(static_cast<Component>(c)) 
	{}
	CameraComponent(CameraComponent&&c) noexcept :
		CameraID(std::move(c.CameraID)),
		ProjectionMatrix(std::move(c.ProjectionMatrix)),
		Orientation(std::move(c.Orientation)),
		Front(std::move(c.Front)),
		Right(std::move(c.Right)),
		Up(std::move(c.Up)),
		Fov(std::move(c.Fov)),
		NearPlane(std::move(c.NearPlane)),
		FarPlane(std::move(c.FarPlane)),
		Component(std::move(c)) 
	{}

	CameraComponent& operator=(const CameraComponent& c) 
	{
		CameraID = c.CameraID;
		ProjectionMatrix = c.ProjectionMatrix;
		Orientation = c.Orientation;
		Front = c.Front;
		Right = c.Right;
		Up = c.Up;
		Fov = c.Fov;
		NearPlane = c.NearPlane;
		FarPlane = c.FarPlane;
		return *this;
	}

	int CameraID;

	glm::mat4 ProjectionMatrix;
	glm::mat4 Orientation;

	glm::vec3 Front;
	glm::vec3 Right;
	glm::vec3 Up;

	float Fov;
	float NearPlane;
	float FarPlane;

	inline static const std::string ComponentName = "CameraComponent";
	inline static int ComponentIndex = -1;
};
