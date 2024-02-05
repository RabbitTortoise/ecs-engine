#pragma once

/// <summary>
/// All components inherit from this class.
/// When inheriting this class, always make at least a copy-constructor that also calls this class's copy constructor. Even better if you also make move constructor and a operator= override.
/// Failing to provide correct constructors causes undefined behaviour.
/// </summary>
class Component
{

public:
	Component() {}
	Component(Component const& c) {}
	Component(Component &&c) noexcept {}
	Component& operator=(const Component& c) { return *this; }

	~Component() {}
};


/* Example of a correctly inherited Component.
class PositionComponent : public Component
{
public:
	PositionComponent() : Component() { Console::Log("PositionComponent()"); }
	PositionComponent(const PositionComponent &c) : Component(static_cast<Component>(c)), X(c.X), Y(c.Y), Z(c.Z) {}
	PositionComponent(PositionComponent &&c) noexcept : Component(std::move(c)), X(std::move(c.X)), Y(std::move(c.Y)), Z(std::move(c.Z)) {}

	PositionComponent& operator=(const PositionComponent& c) { X = c.X; Y = c.Y; Z = c.Z; return *this; }

	float X = 0.0f;
	float Y = 0.0f;
	float Z = 0.0f;
};
*/