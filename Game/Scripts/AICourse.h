#pragma once
#include <Engine/Core/ScriptingCore.h>
#include <vector>
#include <glm/glm.hpp>
#include <chrono>
#include <Game/Scripts/AICourseGameManager.h>

class EntityManager;
class SystemsManager;
class MeshComponent;

class MeshLoaderSystem;
class TextureLoaderSystem;
class ShaderLoaderSystem;
class CameraControllerSystem;
class CollectorAgentSystem;
class AttackerAgentSystem;



class AICourse : public BaseScript
{
	ADDSCRIPT(AICourse);

public:
	AICourse();
	~AICourse();

	void Init() override;
	void ReadInput();
	void Update(float deltaTime) override;

private:
	EntityManager* WorldEntityManager;
	SystemsManager* WorldSystemsManager;

	MeshLoaderSystem* MeshLoader;
	TextureLoaderSystem* TextureLoader;
	ShaderLoaderSystem* ShaderLoader;
	CameraControllerSystem* CameraController;
	CollectorAgentSystem* CollectorSystem;
	AttackerAgentSystem* AttackerSystem;

	AICourseGameManager GameManager;

	std::vector<std::vector<int>> BaseGrid;

	glm::vec2 CursorPos;
	void SetCursorPos();
	int CursorID;
	int CursorTypeID;
	float CursorSpeed = 20.0f;

	//Constant time repeat presses
	bool CanConstantPress = true;
	std::chrono::time_point<std::chrono::system_clock> LastTime;

	//Key presses
	bool PressedNum1 = false;
	bool PressedNum2 = false;
	bool PressedNum3 = false;

	float DeltaTime;

	int HumanPlayerIndex = 0;
	int AIPlayerIndex = 1;


	float RandomNumber0_1();
};