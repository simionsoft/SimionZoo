#pragma once

class Bindable
{
public:
	virtual void update(std::string key, double value)= 0;
};

class Binding
{
public:
	std::string externalName;
	std::string internalName;
	Bindable* pObj;
	Binding(std::string _externalName,std::string _internalName, Bindable* _pObj)
		:externalName(_externalName),internalName(_internalName),pObj(_pObj){}
};