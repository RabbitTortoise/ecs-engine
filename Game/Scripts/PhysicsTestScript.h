#pragma once

#include <Engine/Core/ScriptingCore.h>
#include <Engine/OpenGL/Shader.h>
#include <map>
#include <chrono>

class EntityManager;
class SystemsManager;
class MeshComponent;

class PhysicsTestScript : public BaseScript
{
	ADDSCRIPT(PhysicsTestScript);

public:
	PhysicsTestScript();
	~PhysicsTestScript();

	void Init() override;
	void Update(float deltaTime) override;

	void SpawnCollider();

private:


	EntityManager* WorldEntityManager;
	SystemsManager* WorldSystemsManager;

	Shader* PlayerShader;		// Pointer to the Shader object

	int EntityTypeID = -1;
	int Collider1ID = -1;
	int Collider2ID = -1;

	std::map<int, float> Particles;
	float MaxLifeTime = 5.0f;
	float LastTime = 0.0f;
	float SpawnRate = 0.75f;

	bool pressed = false;


	class CustomTimer {
	public:
		CustomTimer()
			: start(std::chrono::steady_clock::now()),
			deltaStart(start) {
		}

		float getDeltaTime() {
			auto end = std::chrono::steady_clock::now();
			float deltaTime = std::chrono::duration_cast<std::chrono::duration<float>>(end - deltaStart).count();
			deltaStart = end;
			return deltaTime;
		}

		float getRunTime()
		{
			auto end = std::chrono::steady_clock::now();
			float runTime = std::chrono::duration_cast<std::chrono::duration<float>>(end - start).count();
			return runTime;
		}


	private:
		CustomTimer(const CustomTimer&) = delete;
		std::chrono::time_point<std::chrono::steady_clock> start;
		std::chrono::time_point<std::chrono::steady_clock> deltaStart;
	};

	CustomTimer Timer;

};