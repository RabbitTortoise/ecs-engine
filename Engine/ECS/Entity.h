#pragma once
#include <Engine/ECS/Component.h>
#include <vector>
#include <string>

class Entity
{
	friend class EntityManager;

public:
	Entity() { ID = -1; Type = -1; bEnabled = true; };
	Entity(int id, int type, bool enabled = true) : ID(id), Type(type), bEnabled(enabled) { }
	Entity(Entity const& e) : ID(e.ID), Type(e.Type), bEnabled(e.bEnabled) {}
	Entity(Entity&& e) noexcept : ID(std::move(e.ID)), Type(std::move(e.Type)), bEnabled(std::move(e.bEnabled)) {}
	Entity& operator=(const Entity& e) { ID = e.ID; Type = e.Type; bEnabled = e.bEnabled; return *this; }
	~Entity() {}

	int GetID() { return ID; }
	int GetType() { return Type; }
	bool GetEnabled() { return bEnabled; }

protected:

	template<typename T>
	T* FetchComponent(int componentIndex)
	{
		T* c = static_cast<T*>(GetComponent(componentIndex));
		return c;
	}
	virtual Component* GetComponent(int componentName) = 0;

	void SetID(int id) { ID = id; }
	void SetType(int type) { Type = type; }
	void SetEnabled(bool enabled) { bEnabled = enabled; }

private:
	int ID;
	int Type;
	bool bEnabled;
};







//Key* Getter::TemplateGet<Value, Component>(std::string test) { return nullptr; };
//template<> PositionComponent* PlayerEntity::ComponentFetch<PositionComponent>() { return &Position; };



//#define ADDSCRIPT(CLASSNAME)				private: inline static bool Success = Add_Script<CLASSNAME>(#CLASSNAME);
//template<typename T>
// template
// 
// template <class E, class T>
//class ComponentGet
//{
//	T* operator()(E* e)
//	{
//		//T* c = e->ComponentFetch<T>();
//		return c;
//	}
//};
// 
// typedef Component* (*TemplateGet)();
// #define ADDGETCOMPONENT(COMPONENTTYPE, COMPONENTVAR)	public: <template>inline COMPONENTTYPE* GetComponent<COMPONENTTYPE>() { return static_cast<COMPONENTTYPE>();};
/*

template<> PositionComponent* PlayerEntity::ComponentFetch<PositionComponent>() { return &Position; };
template<> SpeedComponent* PlayerEntity::ComponentFetch<SpeedComponent>() { return &Speed; };
template<> PositionComponent* Entity::Getter<PositionComponent>::GetComponent(Entity* e) { return static_cast<PlayerEntity*>(e)->ComponentFetch<PositionComponent>(); }
template<> SpeedComponent* Entity::Getter<SpeedComponent>::GetComponent(Entity* e) { return static_cast<PlayerEntity*>(e)->ComponentFetch<SpeedComponent>(); }
*/


	