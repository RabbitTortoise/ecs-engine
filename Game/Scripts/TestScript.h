#pragma once

#include <Engine/Core/ScriptingCore.h>
#include <Engine/OpenGL/Shader.h>

class EntityManager;
class SystemsManager;
class MeshComponent;

class TestScript : public BaseScript
{
	//ADDSCRIPT(TestScript);

public:
	TestScript();
	~TestScript();

	void Init() override;
	void Update(float deltaTime) override;

private:


	EntityManager* WorldEntityManager;
	SystemsManager* WorldSystemsManager;

	Shader* PlayerShader;		// Pointer to the Shader object
};