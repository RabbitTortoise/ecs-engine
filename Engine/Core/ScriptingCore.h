#pragma once

#include <Engine/Core/BaseScript.h>
#include <string>
#include <map>
#include <vector>
#include <iostream>


typedef BaseScript* (*SpawnFunction)();

class ScriptingCore
{

public:
	ScriptingCore() 
	{
		
	};
	~ScriptingCore() 
	{ 
		for (auto s : LoadedScripts)
		{
			delete s;
			s = nullptr;
		}
	};

	void CreateScripts()
	{	
		typedef std::map<std::string, SpawnFunction>::iterator it_type;
		for (it_type iterator = ScriptInvokerMap.begin(); iterator != ScriptInvokerMap.end(); iterator++)
		{
			BaseScript* script = iterator->second();
			LoadedScripts.push_back(script);
		}	
	}

	void InitScripts()
	{
		for (auto s : LoadedScripts)
		{
			s->Init();
		}
	}

	void UpdateScripts(float deltaTime)
	{
		for (auto s : LoadedScripts)
		{
			s->Update(deltaTime);
		}
	}


	//Returns reference to the invoker function map
	inline static std::map<std::string, SpawnFunction>& GetScriptInvokerMap() { return ScriptingCore::ScriptInvokerMap; }

private:
	std::vector<BaseScript*> LoadedScripts;
	//Holds script invoker functions used in script creation.
	inline static std::map<std::string, SpawnFunction> ScriptInvokerMap;

};

namespace ScriptInvoker {

	template <typename T>
	BaseScript* InvokeCreate()
	{
		T* newScript = new T();
		BaseScript* base = dynamic_cast<BaseScript*>(newScript);
		return base;
	}
}

template <typename T>
bool Add_Script(std::string name)
{
	ScriptingCore::GetScriptInvokerMap().emplace(name, ScriptInvoker::InvokeCreate<T>);
	return true;
}

#define ADDSCRIPT(CLASSNAME)				private: inline static bool Success = Add_Script<CLASSNAME>(#CLASSNAME);
