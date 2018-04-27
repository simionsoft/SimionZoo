#pragma once
#include <vector>
#include <initializer_list>
#include <memory>
#include <map>
#include <functional>
#include <list>
#include <tuple>
#include "config.h"
#ifdef _WIN64
#include "CNTKWrapperLoader.h"

#endif

using namespace std;
//Enumerated types
enum class Distribution { linear, quadratic, cubic };
enum class Interpolation { linear, quadratic, cubic };
enum class TimeReference { episode, experiment };

template<typename DataType>
class SimpleParam
{
protected:
	const char* m_name;
	const char* m_comment;
	DataType m_value;
	DataType m_default;

	void initValue(ConfigNode* pConfigNode, int& value)
	{
		value = pConfigNode->getConstInteger(m_name, m_default);
	}
	void initValue(ConfigNode* pConfigNode, double& value)
	{
		value = pConfigNode->getConstDouble(m_name, m_default);
	}
	void initValue(ConfigNode* pConfigNode, const char*& value)
	{
		value = pConfigNode->getConstString(m_name, m_default);
	}
	void initValue(ConfigNode* pConfigNode, bool& value)
	{
		value = pConfigNode->getConstBoolean(m_name, m_default);
	}
	void initValue(ConfigNode* pConfigNode, TimeReference& value)
	{
		const char* strValue = pConfigNode->getConstString(m_name);
		if (!strcmp(strValue, "episode"))
		{
			value = TimeReference::episode; return;
		}
		else if (!strcmp(strValue, "experiment"))
		{
			value = TimeReference::experiment; return;
		}
		value = m_default;
	}
	void initValue(ConfigNode* pConfigNode, Distribution& value)
	{
		const char* strValue = pConfigNode->getConstString(m_name);
		if (!strcmp(strValue, "linear"))
		{
			value = Distribution::linear;
			return;
		}
		else if (!strcmp(strValue, "quadratic"))
		{
			value = Distribution::quadratic;
			return;
		}
		else if (!strcmp(strValue, "cubic"))
		{
			value = Distribution::cubic;
			return;
		}
		value = m_default;
	}
	void initValue(ConfigNode* pConfigNode, Interpolation& value)
	{
		const char* strValue = pConfigNode->getConstString(m_name);
		if (!strcmp(strValue, "linear"))
		{
			value = Interpolation::linear; return;
		}
		else if (!strcmp(strValue, "quadratic"))
		{
			value = Interpolation::quadratic; return;
		}
		else if (!strcmp(strValue, "cubic"))
		{
			value = Interpolation::cubic; return;
		}
		value = m_default;
	}
public:
	SimpleParam() = default;
	SimpleParam(ConfigNode* pConfigNode
		, const char* name, const char* comment, DataType default)
	{
		m_name = name;
		m_comment = comment;
		m_default = default;

		initValue(pConfigNode, m_value);
	}

	DataType get() const { return m_value; }
	void set(DataType value) { m_value = value; }
};

using INT_PARAM = SimpleParam<int>;
using DOUBLE_PARAM = SimpleParam<double>;
using BOOL_PARAM = SimpleParam<bool>;
using STRING_PARAM = SimpleParam<const char*>;
using DIR_PATH_PARAM = SimpleParam<const char*>;
using FILE_PATH_PARAM = SimpleParam<const char*>;


template <typename EnumType>
class ENUM_PARAM : public SimpleParam<EnumType>
{
public:
	ENUM_PARAM() = default;

	ENUM_PARAM(ConfigNode* pConfigNode, const char* name, const char* comment, EnumType default)
	{
		m_name = name;
		m_comment = comment;
		m_default = default;
		initValue(pConfigNode, m_value);
	}
};


class STATE_VARIABLE
{
protected:
	const char* m_name = 0;
	const char* m_comment = 0;
	int m_hVariable = -1;
	const char* m_variableName = 0;
public:
	STATE_VARIABLE() = default;

	STATE_VARIABLE(ConfigNode* pConfigNode, const char* name, const char* comment);
	int get() { return m_hVariable; }
	void set(int hVar) { m_hVariable = hVar; }
	const char* getName() { return m_variableName; }
};

class ACTION_VARIABLE
{
	const char* m_name;
	const char* m_comment;
	int m_hVariable = -1;
	const char* m_variableName = 0;
public:
	ACTION_VARIABLE() = default;

	ACTION_VARIABLE(ConfigNode* pConfigNode, const char* name, const char* comment);
	int get() { return m_hVariable; }
	void set(unsigned int index) { m_hVariable = index; }
	const char* getName() { return m_variableName; }
};

template<typename DataType>
class CHILD_OBJECT
{
	const char* m_name = 0;
	const char* m_comment = 0;
	bool m_bOptional = false;
	shared_ptr<DataType> m_pValue;
public:
	CHILD_OBJECT() = default;
	template <typename DataType> CHILD_OBJECT(DataType* value)
	{
		m_pValue = std::shared_ptr<DataType>(value);
	}
	CHILD_OBJECT(ConfigNode* pConfigNode, const char* name, const char* comment
		, bool bOptional = false, const char* badgerMetadata = "")
	{
		m_name = name;
		m_comment = comment;
		m_bOptional = bOptional;

		if (!m_bOptional || pConfigNode->getChild(name))
			m_pValue = std::shared_ptr<DataType>(new DataType(pConfigNode->getChild(name)));
		else if (m_bOptional)
		{
			//we create the object using the parameter-less constructor
			m_pValue = std::shared_ptr<DataType>(new DataType());
		}
	}

	DataType* operator->() { return m_pValue.get(); }
	shared_ptr<DataType> shared_ptr() { return m_pValue; }
	DataType* ptr() { return m_pValue.get(); }
};

template<typename DataType>
class CHILD_OBJECT_FACTORY
{
	const char* m_name = 0;
	const char* m_comment = 0;
	bool m_bOptional = false;
	shared_ptr<DataType> m_pValue = 0;
public:
	CHILD_OBJECT_FACTORY() = default;
	template <typename DataType> CHILD_OBJECT_FACTORY(DataType* value)
	{
		m_pValue = std::shared_ptr<DataType>(value);
	}
	CHILD_OBJECT_FACTORY(ConfigNode* pConfigNode, const char* name, const char* comment
		, bool bOptional = false, const char* badgerMetadata = "")
	{
		m_name = name;
		m_comment = comment;
		m_bOptional = bOptional;

		if (!m_bOptional || pConfigNode->getChild(name))
			m_pValue = DataType::getInstance(pConfigNode->getChild(name));
	}
	void set(DataType* newValue)
	{
		m_pValue = std::shared_ptr<DataType>(newValue);
	}

	DataType* operator->() { return m_pValue.get(); }
	shared_ptr<DataType> shared_ptr() { return m_pValue; }
	DataType* ptr() { return m_pValue.get(); }
};

template <typename DataType>
class MULTI_VALUE
{
protected:
	const char* m_name = 0;
	const char* m_comment = 0;
	bool m_bOptional = false;
	vector<shared_ptr<DataType>> m_values;
public:
	MULTI_VALUE() = default;

	MULTI_VALUE(ConfigNode* pConfigNode, const char* name, const char* comment, bool bOptional = false)
	{
		m_name = name;
		m_comment = comment;
		m_bOptional = bOptional;

		ConfigNode* pChildParameters = pConfigNode->getChild(name);
		while (pChildParameters != 0)
		{
			m_values.push_back(shared_ptr<DataType>(new DataType(pChildParameters)));
			pChildParameters = pChildParameters->getNextSibling(name);
		}
	}
	size_t size() { return m_values.size(); }
	DataType* operator[] (size_t index) { return m_values[index].get(); }
};

template <typename DataType>
class MULTI_VALUE_FACTORY : public MULTI_VALUE<DataType>
{
protected:
	const char* m_name = 0;
	const char* m_comment = 0;
	bool m_bOptional = false;
public:
	MULTI_VALUE_FACTORY() = default;

	MULTI_VALUE_FACTORY(ConfigNode* pConfigNode, const char* name, const char* comment, bool bOptional = false)
	{
		m_name = name;
		m_comment = comment;
		m_bOptional = bOptional;

		ConfigNode* pChildParameters = pConfigNode->getChild(name);
		while (pChildParameters != 0)
		{
			m_values.push_back(shared_ptr<DataType>(DataType::getInstance(pChildParameters)));
			pChildParameters = pChildParameters->getNextSibling(name);
		}
	}
};

template <typename DataType, typename baseDataType>
class MULTI_VALUE_SIMPLE_PARAM : public MULTI_VALUE<DataType>
{
protected:
	const char* m_name = 0;
	const char* m_comment = 0;
public:
	MULTI_VALUE_SIMPLE_PARAM() = default;

	MULTI_VALUE_SIMPLE_PARAM(ConfigNode* pConfigNode, const char* name, const char* comment, baseDataType default)
	{
		m_name = name;
		m_comment = comment;

		ConfigNode* pChildParameters = pConfigNode->getChild(name);
		while (pChildParameters != 0)
		{
			m_values.push_back(shared_ptr<DataType>(new DataType(pChildParameters, name, comment, default)));
			pChildParameters = pChildParameters->getNextSibling(name);
		}
	}
};

template <typename DataType>
class MULTI_VALUE_VARIABLE : public MULTI_VALUE<DataType>
{
protected:
	const char* m_name = 0;
	const char* m_comment = 0;
public:
	MULTI_VALUE_VARIABLE() = default;

	MULTI_VALUE_VARIABLE(ConfigNode* pConfigNode, const char* name, const char* comment)
	{
		m_name = name;
		m_comment = comment;

		ConfigNode* pChildParameters = pConfigNode->getChild(name);
		while (pChildParameters != 0)
		{
			m_values.push_back(shared_ptr<DataType>(new DataType(pChildParameters, name, comment)));
			pChildParameters = pChildParameters->getNextSibling(name);
		}
	}
};

template<typename Class>
shared_ptr<Class> CHOICE_ELEMENT_NEW(ConfigNode* pConfigNode)
{
	return shared_ptr<Class>(new Class(pConfigNode));
};
template<typename Class>
shared_ptr<Class> CHOICE_ELEMENT_FACTORY(ConfigNode* pConfigNode)
{
	return Class::getInstance(pConfigNode);
};

template <typename DataType>
using choiceElement = function<shared_ptr<DataType>(ConfigNode*)>;

template<typename BaseClass>
shared_ptr<BaseClass> CHOICE(ConfigNode* pConfig, const char* choiceName, const char* comment
	, list<tuple<const char*, choiceElement<BaseClass>, const char*>> choices)
{
	if (!pConfig) return 0;
	pConfig = pConfig->getChild(choiceName);
	if (!pConfig) return 0;

	for each (auto var in choices)
	{
		const char* choiceName = get<0>(var);
		ConfigNode* pChoiceConfig = pConfig->getChild(choiceName);
		if (pChoiceConfig != 0)
		{
			choiceElement<BaseClass> choiceConstructor = get<1>(var);
			return choiceConstructor(pChoiceConfig);
		}
	}
	return 0;
}

template<typename BaseClass>
shared_ptr<BaseClass> CHOICE(ConfigNode* pConfig, const char* choiceName, const char* comment
	, map<const char*, choiceElement<BaseClass>> choices)
{
	if (!pConfig) return 0;
	pConfig = pConfig->getChild(choiceName);
	if (!pConfig) return 0;

	for each (auto var in choices)
	{
		const char* choiceName = var.first;
		ConfigNode* pChoiceConfig = pConfig->getChild(choiceName);
		if (pChoiceConfig != 0)
		{
			return var.second(pChoiceConfig);
		}
	}
	return 0;
}

//quick and dirty hack to store the name of the dynamic world in a choice
#define METADATA(name, value) m_name= value;

#ifdef _WIN64

#include "CNTKWrapperLoader.h"
class INetwork;

//Because NN objects are created from the CNTKWrapper DLL, this objects needs to be created using "new" and cannot
//use shared_ptr as in the rest of the objects
//The copy assignment operator is overriden to avoid destroying m_pProblem and m_pNetwork after a copy-construct
class NN_DEFINITION
{
protected:
	INetworkDefinition* m_pDefinition= nullptr;

	const char* m_name;
	const char* m_comment;

public:
	NN_DEFINITION() = default;
	NN_DEFINITION(ConfigNode* pConfigNode, const char* name, const char* comment)
	{
		m_name = name;
		m_comment = comment;
		m_pDefinition = CNTKWrapperLoader::getProblem(pConfigNode->FirstChildElement(m_name)->FirstChildElement("Problem"));
	}
	NN_DEFINITION& operator=(NN_DEFINITION& copied);
	INetworkDefinition* operator->() { return m_pDefinition; }

	~NN_DEFINITION();

	//need to explicitly destroy the object allocated from the CNTK wrapper DLL
	void destroy();
};
#endif