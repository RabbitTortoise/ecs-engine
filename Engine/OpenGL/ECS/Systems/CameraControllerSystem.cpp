#include <CameraControllerSystem.h>
#include <MatrixCalculations.h>

#include <Engine/ECS/Components/TransformComponent.h>
#include <Engine/OpenGL/ECS/Components/CameraComponent.h>
#include <Engine/ECS/Components/MovementComponent.h>

#include <Engine/OpenGL/ECS/Entities/CameraEntity.h>

#include <Engine/Interface/WindowManager.h>

#include <iostream>

CameraControllerSystem::CameraControllerSystem(EntityManager* entityManager, int cameraEntityTypeID) : IComponentTypeQuerySystem(entityManager)
{
	CameraCount = 0;
	MainCameraID = 0;

	CameraEntityTypeID = cameraEntityTypeID;
}

CameraControllerSystem::~CameraControllerSystem()
{
}

void CameraControllerSystem::Init()
{
}

void CameraControllerSystem::Destroy()
{
}

void CameraControllerSystem::PreUpdate(float deltaTime)
{
}

void CameraControllerSystem::ComponentTypeQueryUpdate(float deltaTime, IEntityStorage* entityTypeStorage)
{
	EntityTypeStorage<Entity>* entityStorage = static_cast<EntityTypeStorage<Entity>*>(entityTypeStorage);
	//std::cout << "RenderSystem: Entity count: " << std::to_string(entityStorage->GetEntityCount()) << std::endl;
	GLFWwindow* window = WindowManager::GetGLFWWindow();

	int windowWidth = WindowManager::GetWindowWidth();
	int windowHeight = WindowManager::GetWindowHeight();

	glm::vec3 mousePosDifference = {0,0, 0};

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2))
	{
		double x, y;
		float posX, posY;
		glfwGetCursorPos(window, &x, &y);
		posX = static_cast<float>(x);	posY = static_cast<float>(y);

		mousePosDifference.x = LastMousePos.x - posX;
		mousePosDifference.y = LastMousePos.y - posY;

		if (RightButtonJustClicked) { mousePosDifference.x = 0; mousePosDifference.y = 0; RightButtonJustClicked = false; }

		LastMousePos.x = posX;
		LastMousePos.y = posY;

		Yaw -= mousePosDifference.x;
		Pitch += mousePosDifference.y;

		bool constrainPitch = true;
		if (constrainPitch)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}
	}
	else { RightButtonJustClicked = true; }

	for (int i = 0; i < entityStorage->GetEntityCount(); i++)
	{
		Entity* e = entityStorage->GetEntity(i);
		if (e != nullptr)
		{
			TransformComponent* transformComponent = WorldEntityManager->GetComponent<TransformComponent>(e);
			CameraComponent* cameraComponent = WorldEntityManager->GetComponent<CameraComponent>(e);
			MovementComponent* movementComponent = WorldEntityManager->GetComponent<MovementComponent>(e);


			if (transformComponent != nullptr && cameraComponent != nullptr && movementComponent != nullptr)
			{
				glm::vec3 WorldUp = glm::vec3(0, 1, 0);
				glm::vec3 front;
				front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
				front.y = sin(glm::radians(Pitch));
				front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
				cameraComponent->Front = glm::normalize(front);
				cameraComponent->Right = glm::normalize(glm::cross(cameraComponent->Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
				cameraComponent->Up = glm::normalize(glm::cross(cameraComponent->Right, cameraComponent->Front));

				
				movementComponent->Direction = glm::vec3(0, 0, 0);
				float speedMultiplier = 1.0f;
				if (glfwGetKey(window, GLFW_KEY_W)) { movementComponent->Direction += cameraComponent->Front; }
				if (glfwGetKey(window, GLFW_KEY_S)) { movementComponent->Direction += -cameraComponent->Front; }
				if (glfwGetKey(window, GLFW_KEY_A)) { movementComponent->Direction += -cameraComponent->Right; }
				if (glfwGetKey(window, GLFW_KEY_D)) { movementComponent->Direction += cameraComponent->Right; }
				if (glfwGetKey(window, GLFW_KEY_Q)) { movementComponent->Direction += -WorldUp; }
				if (glfwGetKey(window, GLFW_KEY_E)) { movementComponent->Direction += WorldUp; }
				if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)) { speedMultiplier += 2.0f; }
				if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL)) { speedMultiplier -= 0.5f; }
				glm::normalize(movementComponent->Direction);

				//std::cout << transformComponent->Position.x << "," << transformComponent->Position.y << "," << transformComponent->Position.z << std::endl;
				transformComponent->Position += movementComponent->Direction * movementComponent->Speed * speedMultiplier * deltaTime;
				//std::cout << transformComponent->Position.x << "," << transformComponent->Position.y << "," << transformComponent->Position.z << std::endl;

				cameraComponent->Orientation = MatrixCalculations::CalculateLookAtMatrix(transformComponent->Position, transformComponent->Position + cameraComponent->Front, cameraComponent->Up);
				
			}
		}
	}
}

int CameraControllerSystem::CreateCamera(int width, int height, bool ortho, float fovAngle, float near, float far)
{
	CameraEntity* camera = WorldEntityManager->CreateEntity<CameraEntity>();


	TransformComponent* transformComponent = WorldEntityManager->GetComponent<TransformComponent>(camera);
	CameraComponent* cameraComponent = WorldEntityManager->GetComponent<CameraComponent>(camera);
	MovementComponent* movementComponent = WorldEntityManager->GetComponent<MovementComponent>(camera);

	if (transformComponent != nullptr && cameraComponent != nullptr && movementComponent != nullptr)
	{
		transformComponent->Position = glm::vec3(0.0f, 2.0f, 0.0f);
		transformComponent->Scale = glm::vec3(1.0f, 1.0f, 1.0f);
		transformComponent->EulerAngles = glm::vec3(0.0f, 0.0f, 0.0f);
		transformComponent->Orientation = glm::quat(0.0f, 0.0f, 0.0f, 1.0f);

		cameraComponent->Front = glm::vec3(0.0f, 2.0f, 1.0f);
		cameraComponent->Orientation = MatrixCalculations::CalculateLookAtMatrix(transformComponent->Position, transformComponent->Position + cameraComponent->Front, cameraComponent->Up);
		cameraComponent->Fov = fovAngle;
		cameraComponent->NearPlane = near;
		cameraComponent->FarPlane = far;
		cameraComponent->CameraID;
		if (ortho)
		{
			cameraComponent->ProjectionMatrix = glm::ortho(-width / 2.0f, width / 2.0f, -height / 2.0f, height / 2.0f);
		}
		else
		{
			cameraComponent->ProjectionMatrix = glm::perspective(glm::radians(fovAngle), static_cast<float>(width) / static_cast<float>(height), near, far);
		}

		movementComponent->Direction = glm::vec3(0.0f, 0.0f, 0.0f);
		movementComponent->Speed = 2.0f;
	}

	CameraEntities.push_back(camera->GetID());
	CameraCount++;

	return CameraCount - 1;
}

void CameraControllerSystem::UpdateCameraProjectionMatrix(CameraEntity* camera, int width, int height, bool ortho)
{
	CameraComponent* cameraComponent = WorldEntityManager->GetComponent<CameraComponent>(camera);

	if (cameraComponent != nullptr)
	{
		if (ortho)
		{
			cameraComponent->ProjectionMatrix = glm::ortho(-width / 2.0f, width / 2.0f, -height / 2.0f, height / 2.0f);
		}
		else
		{
			cameraComponent->ProjectionMatrix = glm::perspective(glm::radians(cameraComponent->Fov), static_cast<float>(width) / static_cast<float>(height), cameraComponent->NearPlane, cameraComponent->FarPlane);
		}
	}
}



void CameraControllerSystem::SetMainCamera(int cameraID)
{
	MainCameraID = cameraID;
}

CameraEntity* CameraControllerSystem::GetMainCamera()
{
	return WorldEntityManager->GetEntity<CameraEntity>(MainCameraID, CameraEntityTypeID);
}

void CameraControllerSystem::SetPitchYaw(float pitch, float yaw)
{
	Pitch = pitch;
	Yaw = yaw;
}
