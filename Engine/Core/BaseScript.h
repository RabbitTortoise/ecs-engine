#pragma once


class BaseScript
{
public: 
	BaseScript() {};
	~BaseScript() {};

	virtual void Init() {};
	virtual void Update(float deltaTime) {};
};

/*
template<class T>
class ScriptRef : BaseScript
{
public:
	ScriptRef() {};
	ScriptRef(T* ptr) : Pointer(ptr) {}
	~ScriptRef()
	{
		delete Pointer;
		Pointer = nullptr;
	};

	operator T* () const { return Pointer; }
	T* GetPointer() const { return Pointer; }

protected:
	T* Pointer;
};
*/