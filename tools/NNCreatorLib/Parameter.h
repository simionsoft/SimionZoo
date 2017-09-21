#pragma once
#include <string>
#include "../../3rd-party/tinyxml2/tinyxml2.h"
#include "ParameterValues.h"
#include <vector>

using namespace std;

class CParameterBaseInteface
{
protected:
	string m_name;
public:
	string getName() { return m_name; }
	void setName(string name) { m_name = name; }
	static CParameterBaseInteface* getInstance(tinyxml2::XMLElement* pNode);
};

template <typename T>
class CParameterBase : public CParameterBaseInteface
{
protected:
	T m_value;
	CParameterBase(tinyxml2::XMLElement* pNode);

public:
	CParameterBase() : CParameterBaseInteface() {};
	~CParameterBase();

	T getValue() const { return m_value; }
	void setValue(T value) { m_value = value; }

};

class CDoubleParameter :public CParameterBase<double>
{
public:
	CDoubleParameter(tinyxml2::XMLElement* pNode);

};

class CIntParameter : public CParameterBase<int>
{
public:
	CIntParameter(tinyxml2::XMLElement* pNode);
	CIntParameter() : CParameterBase<int>() {};

};

class CIntTuple1DParameter : public CParameterBase<CIntTuple1D>
{
public:
	CIntTuple1DParameter(tinyxml2::XMLElement* pNode);

};

class CIntTuple2DParameter : public CParameterBase<CIntTuple2D>
{
public:
	CIntTuple2DParameter(tinyxml2::XMLElement* pNode);

};

class CIntTuple3DParameter : public CParameterBase<CIntTuple3D>
{
public:
	CIntTuple3DParameter(tinyxml2::XMLElement* pNode);

};

class CIntTuple4DParameter : public CParameterBase<CIntTuple4D>
{
public:
	CIntTuple4DParameter(tinyxml2::XMLElement* pNode);
};

class CActivationFunctionParameter : public CParameterBase<ActivationFunction>
{
public:
	CActivationFunctionParameter(tinyxml2::XMLElement* pNode);

};

class CInputDataParameter : public CParameterBase<string>
{
public:
	CInputDataParameter(tinyxml2::XMLElement* pNode);

};

class CLinkConnectionListParameter : public CParameterBase<vector<CLinkConnection*>>
{
public:
	CLinkConnectionListParameter(tinyxml2::XMLElement* pNode);

};