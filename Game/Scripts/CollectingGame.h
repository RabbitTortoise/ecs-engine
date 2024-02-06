#pragma once
#include <Engine/Core/ScriptingCore.h>
#include <vector>

class EntityManager;
class SystemsManager;
class MeshComponent;
class TriggerSystem;

class MeshLoaderSystem;
class TextureLoaderSystem;
class ShaderLoaderSystem;
class CameraControllerSystem;

class StaticSceneEntity;


class CollectingGame : public BaseScript
{
	//ADDSCRIPT(CollectingGame);

public:
	CollectingGame();
	~CollectingGame();

	void Init() override;
	void Update(float deltaTime) override;

	void SpawnTrigger();
	void AddPoint();
	void AddCaughtSphere();

private:
	EntityManager* WorldEntityManager;
	SystemsManager* WorldSystemsManager;
	TriggerSystem* GameTriggerSystem;

	MeshLoaderSystem* MeshLoader;
	TextureLoaderSystem* TextureLoader;
	ShaderLoaderSystem* ShaderLoader;
	CameraControllerSystem* CameraController;

	int Points;
	std::vector<StaticSceneEntity> CaughtSpheres;

	float RandomNumber0_1();
};