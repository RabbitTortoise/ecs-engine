#pragma once

#include <Engine/ECS/System.h>			// System declaration
#include <glm/glm.hpp>

class CameraEntity;

class CameraControllerSystem : public IComponentTypeQuerySystem
{
public:
	CameraControllerSystem(EntityManager* entityManager, int cameraEntityTypeID);
	~CameraControllerSystem();
	
	//System class overrides
	void Init() override;
	void Destroy() override;
	void PreUpdate(float deltaTime) override;
	void ComponentTypeQueryUpdate(float deltaTime, IEntityStorage* entityTypeStorage) override;

	int CreateCamera(int width, int height, bool ortho = true, float fovAngle = 45.0f, float near = 0.1f, float far = 100.0f);
	void UpdateCameraProjectionMatrix(CameraEntity* camera, int width, int height, bool ortho = true);
	void SetMainCamera(int cameraID);
	CameraEntity* GetMainCamera();
	void SetPitchYaw(float pitch, float yaw);

	
	inline static const std::string SystemName = "CameraControllerSystem";
	inline static const System::SystemType SystemType = System::SystemType::System;
private:
	int CameraCount;
	int MainCameraID;

	int CameraEntityTypeID;
	std::vector<int> CameraEntities;

	//Debug Camera Movement
	glm::vec2 LastMousePos = {0,0};
	bool RightButtonJustClicked = true;
	float Yaw = -90;
	float Pitch = 0;
};
