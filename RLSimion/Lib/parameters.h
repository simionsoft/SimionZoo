#pragma once
#include <vector>
#include <initializer_list>
#include <memory>
#include <map>
#include <functional>
#include <list>
#include <tuple>
#include "config.h"
#include "deep-layer.h" //Activation enum type is defined there
#include <stdexcept>

using namespace std;
//Enumerated types
enum class Distribution { linear, quadratic, cubic };
enum class Interpolation { linear, quadratic, cubic };
enum class TimeReference { episode, experiment };
enum class UseAsController { Always, OnTraining, OnEvaluations };

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
		if (!strcmp(strValue, "episode")) value = TimeReference::episode;
		else if (!strcmp(strValue, "experiment")) value = TimeReference::experiment;
		else value = m_default;
	}
	void initValue(ConfigNode* pConfigNode, Distribution& value)
	{
		const char* strValue = pConfigNode->getConstString(m_name);
		if (!strcmp(strValue, "linear")) value = Distribution::linear;
		else if (!strcmp(strValue, "quadratic")) value = Distribution::quadratic;
		else if (!strcmp(strValue, "cubic")) value = Distribution::cubic;
		else value = m_default;
	}
	void initValue(ConfigNode* pConfigNode, Interpolation& value)
	{
		const char* strValue = pConfigNode->getConstString(m_name);
		if (!strcmp(strValue, "linear")) value = Interpolation::linear;
		else if (!strcmp(strValue, "quadratic")) value = Interpolation::quadratic;
		else if (!strcmp(strValue, "cubic")) value = Interpolation::cubic;
		else value = m_default;
	}
	void initValue(ConfigNode* pConfigNode, Activation& value)
	{
		const char* strValue = pConfigNode->getConstString(m_name);
		if (strValue != nullptr)
			value = DeepLayer::activationFromFunctionName(strValue);
		else value = m_default;
	}
	void initValue(ConfigNode* pConfigNode, UseAsController& value)
	{
		const char* strValue = pConfigNode->getConstString(m_name);
		if (!strcmp(strValue, "Always")) value = UseAsController::Always;
		else if (!strcmp(strValue, "OnTraining")) value = UseAsController::OnTraining;
		else if (!strcmp(strValue, "OnEvaluations")) value = UseAsController::OnEvaluations;
		else value = m_default;
	}

public:
	SimpleParam() = default;
	SimpleParam(ConfigNode* pConfigNode
		, const char* name, const char* comment, DataType defaultValue)
	{
		m_name = name;
		m_comment = comment;
		m_default = defaultValue;

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
	ENUM_PARAM(ConfigNode* pConfigNode, const char* name, const char* comment, EnumType defaultValue)
	{
		this->m_name = name;
		this->m_comment = comment;
		this->m_default = defaultValue;
		this->initValue(pConfigNode, this->m_value);
	}
};

class NamedVarProperties;
class Descriptor;

class STATE_VARIABLE
{
protected:
	const char* m_name = 0;
	const char* m_comment = 0;
	const char* m_variableName;
public:
	STATE_VARIABLE() = default;
	STATE_VARIABLE(ConfigNode* pConfigNode, const char* name, const char* comment);
	STATE_VARIABLE(const char* variableName);

	void set(const char* variableName) { this->m_variableName = variableName; }
	const char* get() { return this->m_variableName; }
};

class ACTION_VARIABLE
{
	const char* m_name;
	const char* m_comment;
	const char* m_variableName;
public:
	ACTION_VARIABLE() = default;
	ACTION_VARIABLE(ConfigNode* pConfigNode, const char* name, const char* comment);
	ACTION_VARIABLE(const char* variableName);

	void set(const char* variableName) { this->m_variableName = variableName; }
	const char* get() { return this->m_variableName; }
};

class WIRE_CONNECTION
{
	const char* m_name;
	const char* m_comment;
public:
	WIRE_CONNECTION() = default;
	WIRE_CONNECTION(ConfigNode* pConfigNode, const char* name, const char* comment);
	WIRE_CONNECTION(const char* wireName);

	double get();
	void set(double value);
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
	CHILD_OBJECT(DataType* value)
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

	DataType* operator->() const { return m_pValue.get(); }
	DataType* operator->() { return m_pValue.get(); }
	shared_ptr<DataType> sharedPtr() { return m_pValue; }
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
	CHILD_OBJECT_FACTORY(DataType* value)
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
	//An alternative constructor to provide a default value
	CHILD_OBJECT_FACTORY(ConfigNode* pConfigNode, const char* name, const char* comment
		, DataType* defaultValue)
	{
		m_name = name;
		m_comment = comment;

		if (pConfigNode->getChild(name))
		{
			//the initialization of the object should include the call to new() and we need to free it:
			//m_parameter = CHILD_OBJECT_FACTORY<BaseClass> (..., new SubClass());
			if (defaultValue) delete defaultValue;
			m_pValue = DataType::getInstance(pConfigNode->getChild(name));
		}
		else
			m_pValue = std::shared_ptr<DataType>(defaultValue);
	}
	void set(DataType* newValue)
	{
		m_pValue = std::shared_ptr<DataType>(newValue);
	}

	DataType* operator->() const { return m_pValue.get(); }
	DataType* operator->() { return m_pValue.get(); }
	shared_ptr<DataType> sharedPtr() { return m_pValue; }
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
	size_t size() const { return m_values.size(); }
	DataType* operator[] (size_t index) { return m_values[index].get(); }
	void add(DataType* pObj) { m_values.push_back(shared_ptr<DataType>(pObj)); }
};

template <typename DataType>
class MULTI_VALUE_FACTORY : public MULTI_VALUE<DataType>
{
public:
	MULTI_VALUE_FACTORY() = default;

	MULTI_VALUE_FACTORY(ConfigNode* pConfigNode, const char* name, const char* comment, bool bOptional = false)
	{
		this->m_name = name;
		this->m_comment = comment;
		this->m_bOptional = bOptional;

		ConfigNode* pChildParameters = pConfigNode->getChild(name);
		while (pChildParameters != 0)
		{
			this->m_values.push_back(shared_ptr<DataType>(DataType::getInstance(pChildParameters)));
			pChildParameters = pChildParameters->getNextSibling(name);
		}
	}
};

template <typename DataType, typename baseDataType>
class MULTI_VALUE_SIMPLE_PARAM : public MULTI_VALUE<DataType>
{
public:
	MULTI_VALUE_SIMPLE_PARAM() = default;

	MULTI_VALUE_SIMPLE_PARAM(ConfigNode* pConfigNode, const char* name, const char* comment, baseDataType defaultValue)
	{
		this->m_name = name;
		this->m_comment = comment;

		ConfigNode* pChildParameters = pConfigNode->getChild(name);
		while (pChildParameters != 0)
		{
			this->m_values.push_back(shared_ptr<DataType>(new DataType(pChildParameters, name, comment, defaultValue)));
			pChildParameters = pChildParameters->getNextSibling(name);
		}
	}
};

template <typename DataType>
class MULTI_VALUE_VARIABLE : public MULTI_VALUE<DataType>
{
public:
	MULTI_VALUE_VARIABLE() = default;

	MULTI_VALUE_VARIABLE(ConfigNode* pConfigNode, const char* name, const char* comment)
	{
		this->m_name = name;
		this->m_comment = comment;

		ConfigNode* pChildParameters = pConfigNode->getChild(name);
		while (pChildParameters != 0)
		{
			this->m_values.push_back(shared_ptr<DataType>(new DataType(pChildParameters, name, comment)));
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

	for (auto var : choices)
	{
		const char* choiceName = get<0>(var);
		ConfigNode* pChoiceConfig = pConfig->getChild(choiceName);
		if (pChoiceConfig != 0)
		{
			choiceElement<BaseClass> choiceConstructor = get<1>(var);
			return choiceConstructor(pChoiceConfig);
		}
	}
	throw std::runtime_error("Couldn't read the value of a CHOICE parameter from the configuration file.");
}

template<typename BaseClass>
shared_ptr<BaseClass> CHOICE(ConfigNode* pConfig, const char* choiceName, const char* comment
	, map<const char*, choiceElement<BaseClass>> choices)
{
	if (!pConfig) return 0;
	pConfig = pConfig->getChild(choiceName);
	if (!pConfig) return 0;

	for (auto var : choices)
	{
		const char* choiceName = var.first;
		ConfigNode* pChoiceConfig = pConfig->getChild(choiceName);
		if (pChoiceConfig != 0)
		{
			return var.second(pChoiceConfig);
		}
	}
	throw std::runtime_error("Couldn't read the value of a CHOICE parameter from the configuration file.");
}

//quick and dirty hack to store the name of the dynamic world in a choice
#define METADATA(name, value) m_name= value;
