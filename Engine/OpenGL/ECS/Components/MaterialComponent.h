#pragma once

#include <Engine/ECS/Component.h>
#include <Engine/OpenGL/Shader.h>
#include <glad/gl.h>				// Include glad

class MaterialComponent : public Component
{
	friend class RenderSystem;
	friend class InstancedRenderingSystem;

public:
	MaterialComponent() : Component()
	{
		ShaderID = 0;
		Texture_Albedo = 0;
		Texture_Normal = 0;
		Texture_Depth = 0;
		Texture_Metallic = 0;
		Texture_Roughness = 0;
		Texture_AO = 0;
		TextureRepeat = 1;
		ParallaxScale = 0.01f;
		IBLEnabled = 1;
		HiddenInGame = 0;
	}
	MaterialComponent(const MaterialComponent& c) :

		ShaderID(c.ShaderID),
		Texture_Albedo(c.Texture_Albedo),
		Texture_Normal(c.Texture_Normal),
		Texture_Depth(c.Texture_Depth),
		Texture_Metallic(c.Texture_Metallic),
		Texture_Roughness(c.Texture_Roughness),
		Texture_AO(c.Texture_AO),
		TextureRepeat(c.TextureRepeat),
		ParallaxScale(c.ParallaxScale),
		IBLEnabled(c.IBLEnabled),
		HiddenInGame(c.HiddenInGame),
		Component(static_cast<Component>(c))
	{}
	MaterialComponent(MaterialComponent&& c) noexcept :

		ShaderID(std::move(c.ShaderID)),
		Texture_Albedo(std::move(c.Texture_Albedo)),
		Texture_Normal(std::move(c.Texture_Normal)),
		Texture_Depth(std::move(c.Texture_Depth)),
		Texture_Metallic(std::move(c.Texture_Metallic)),
		Texture_Roughness(std::move(c.Texture_Roughness)),
		Texture_AO(std::move(c.Texture_AO)),
		TextureRepeat(std::move(c.TextureRepeat)),
		ParallaxScale(std::move(c.ParallaxScale)),
		IBLEnabled(std::move(c.IBLEnabled)),
		HiddenInGame(std::move(c.HiddenInGame)),
		Component(std::move(c))
	{}

	MaterialComponent& operator=(const MaterialComponent& c)
	{
		ShaderID = c.ShaderID;
		Texture_Albedo = c.Texture_Albedo;
		Texture_Normal = c.Texture_Normal;
		Texture_Depth = c.Texture_Depth;
		Texture_Metallic = c.Texture_Metallic;
		Texture_Roughness = c.Texture_Roughness;
		Texture_AO = c.Texture_AO;
		TextureRepeat = c.TextureRepeat;
		ParallaxScale = c.ParallaxScale;
		IBLEnabled = c.IBLEnabled;
		HiddenInGame = c.HiddenInGame;
		return *this;
	}

	GLuint ShaderID;
	GLuint Texture_Albedo;
	GLuint Texture_Depth;
	GLuint Texture_Normal;
	GLuint Texture_Metallic;
	GLuint Texture_Roughness;
	GLuint Texture_AO;

	float TextureRepeat;
	float ParallaxScale;
	bool IBLEnabled;

	bool HiddenInGame;

	inline static const std::string ComponentName = "MaterialComponent";
	inline static int ComponentIndex = -1;
};
