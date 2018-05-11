#pragma once
#include <string>
#include "../../3rd-party/tinyxml2/tinyxml2.h"
#include "ParameterValues.h"
#include <vector>


using namespace std;

class IParameter
{
protected:
	string m_name;
public:
	string getName() { return m_name; }
	void setName(string name) { m_name = name; }
	static IParameter* getInstance(tinyxml2::XMLElement* pNode);
};

template <typename T>
class ParameterBase : public IParameter
{
protected:
	T m_value;
	ParameterBase(tinyxml2::XMLElement* pNode);

public:
	ParameterBase() : IParameter() {};
	~ParameterBase();

	T getValue() const { return m_value; }
	void setValue(T value) { m_value = value; }

};

class DoubleParameter :public ParameterBase<double>
{
public:
	DoubleParameter(tinyxml2::XMLElement* pNode);

};

class IntParameter : public ParameterBase<int>
{
public:
	IntParameter(tinyxml2::XMLElement* pNode);
	IntParameter() : ParameterBase<int>() {};

};

class IntTuple1DParameter : public ParameterBase<CIntTuple1D>
{
public:
	IntTuple1DParameter(tinyxml2::XMLElement* pNode);
};

class IntTuple2DParameter : public ParameterBase<CIntTuple2D>
{
public:
	IntTuple2DParameter(tinyxml2::XMLElement* pNode);

};

class IntTuple3DParameter : public ParameterBase<CIntTuple3D>
{
public:
	IntTuple3DParameter(tinyxml2::XMLElement* pNode);

};

class IntTuple4DParameter : public ParameterBase<CIntTuple4D>
{
public:
	IntTuple4DParameter(tinyxml2::XMLElement* pNode);
};

class ActivationFunctionParameter : public ParameterBase<ActivationFunction>
{
public:
	ActivationFunctionParameter(tinyxml2::XMLElement* pNode);

};

class InputDataParameter : public ParameterBase<string>
{
public:
	InputDataParameter(tinyxml2::XMLElement* pNode);

};

class LinkConnectionListParameter : public ParameterBase<vector<LinkConnection*>>
{
public:
	LinkConnectionListParameter(tinyxml2::XMLElement* pNode);

};