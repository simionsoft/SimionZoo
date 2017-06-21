#pragma once

#include <string>
#include <vector>
using namespace std;

class Bindable
{
public:
	virtual void update(std::string key, double value)= 0;
};

class BoundObject
{
public:
	Bindable* pObj;
	string internalName;

	BoundObject(Bindable* pObject, string& name)
		: pObj(pObject), internalName(name) {}
};

class Binding
{
public:
	string externalName;
	vector <BoundObject*> boundObjects;

	Binding(std::string& _externalName, Bindable* _pObj, std::string& _internalName)
		:externalName(_externalName)
	{
		boundObjects.push_back(new BoundObject(_pObj,_internalName));
	}
	~Binding()
	{
		for (auto it = boundObjects.begin(); it != boundObjects.end(); ++it)
			delete (*it);
	}
	unsigned int getNumBoundObjects() const { return boundObjects.size(); }
	BoundObject* getBoundObject(unsigned int i)
	{
		if (i<boundObjects.size())
			return boundObjects[i];
		return nullptr;
	}
	void addBoundObject(BoundObject* pObj)
	{
		boundObjects.push_back(pObj);
	}
};