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
class CSimpleParam
{
protected:
	const char* m_name;
	const char* m_comment;
	DataType m_value;
	DataType m_default;

	void initValue(CConfigNode* pConfigNode, int& value)
	{
		value = pConfigNode->getConstInteger(m_name, m_default);
	}
	void initValue(CConfigNode* pConfigNode, double& value)
	{
		value = pConfigNode->getConstDouble(m_name, m_default);
	}
	void initValue(CConfigNode* pConfigNode, const char*& value)
	{
		value = pConfigNode->getConstString(m_name, m_default);
	}
	void initValue(CConfigNode* pConfigNode, bool& value)
	{
		value = pConfigNode->getConstBoolean(m_name, m_default);
	}
	void initValue(CConfigNode* pConfigNode, TimeReference& value)
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
	void initValue(CConfigNode* pConfigNode, Distribution& value)
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
	void initValue(CConfigNode* pConfigNode, Interpolation& value)
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
	CSimpleParam() = default;
	CSimpleParam(CConfigNode* pConfigNode
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

using INT_PARAM = CSimpleParam<int>;
using DOUBLE_PARAM = CSimpleParam<double>;
using BOOL_PARAM = CSimpleParam<bool>;
using STRING_PARAM = CSimpleParam<const char*>;
using DIR_PATH_PARAM = CSimpleParam<const char*>;
using FILE_PATH_PARAM = CSimpleParam<const char*>;


template <typename EnumType>
class ENUM_PARAM : public CSimpleParam<EnumType>
{
public:
	ENUM_PARAM() = default;

	ENUM_PARAM(CConfigNode* pConfigNode, const char* name, const char* comment, EnumType default)
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
public:
	STATE_VARIABLE() = default;

	STATE_VARIABLE(CConfigNode* pConfigNode, const char* name, const char* comment);
	int get() { return m_hVariable; }
	void set(int hVar) { m_hVariable = hVar; }
};

class ACTION_VARIABLE
{
	const char* m_name;
	const char* m_comment;
	int m_hVariable = -1;
public:
	ACTION_VARIABLE() = default;

	ACTION_VARIABLE(CConfigNode* pConfigNode, const char* name, const char* comment);
	int get() { return m_hVariable; }
	void set(unsigned int index) { m_hVariable = index; }
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
	CHILD_OBJECT(CConfigNode* pConfigNode, const char* name, const char* comment
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
	CHILD_OBJECT_FACTORY(CConfigNode* pConfigNode, const char* name, const char* comment
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

	MULTI_VALUE(CConfigNode* pConfigNode, const char* name, const char* comment, bool bOptional = false)
	{
		m_name = name;
		m_comment = comment;
		m_bOptional = bOptional;

		CConfigNode* pChildParameters = pConfigNode->getChild(name);
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

	MULTI_VALUE_FACTORY(CConfigNode* pConfigNode, const char* name, const char* comment, bool bOptional = false)
	{
		m_name = name;
		m_comment = comment;
		m_bOptional = bOptional;

		CConfigNode* pChildParameters = pConfigNode->getChild(name);
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

	MULTI_VALUE_SIMPLE_PARAM(CConfigNode* pConfigNode, const char* name, const char* comment, baseDataType default)
	{
		m_name = name;
		m_comment = comment;

		CConfigNode* pChildParameters = pConfigNode->getChild(name);
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

	MULTI_VALUE_VARIABLE(CConfigNode* pConfigNode, const char* name, const char* comment)
	{
		m_name = name;
		m_comment = comment;

		CConfigNode* pChildParameters = pConfigNode->getChild(name);
		while (pChildParameters != 0)
		{
			m_values.push_back(shared_ptr<DataType>(new DataType(pChildParameters, name, comment)));
			pChildParameters = pChildParameters->getNextSibling(name);
		}
	}
};

template<typename Class>
shared_ptr<Class> CHOICE_ELEMENT_NEW(CConfigNode* pConfigNode)
{
	return shared_ptr<Class>(new Class(pConfigNode));
};
template<typename Class>
shared_ptr<Class> CHOICE_ELEMENT_FACTORY(CConfigNode* pConfigNode)
{
	return Class::getInstance(pConfigNode);
};

template <typename DataType>
using choiceElement = function<shared_ptr<DataType>(CConfigNode*)>;

template<typename BaseClass>
shared_ptr<BaseClass> CHOICE(CConfigNode* pConfig, const char* choiceName, const char* comment
	, list<tuple<const char*, choiceElement<BaseClass>, const char*>> choices)
{
	if (!pConfig) return 0;
	pConfig = pConfig->getChild(choiceName);
	if (!pConfig) return 0;

	for each (auto var in choices)
	{
		const char* choiceName = get<0>(var);
		CConfigNode* pChoiceConfig = pConfig->getChild(choiceName);
		if (pChoiceConfig != 0)
		{
			choiceElement<BaseClass> choiceConstructor = get<1>(var);
			return choiceConstructor(pChoiceConfig);
		}
	}
	return 0;
}

template<typename BaseClass>
shared_ptr<BaseClass> CHOICE(CConfigNode* pConfig, const char* choiceName, const char* comment
	, map<const char*, choiceElement<BaseClass>> choices)
{
	if (!pConfig) return 0;
	pConfig = pConfig->getChild(choiceName);
	if (!pConfig) return 0;

	for each (auto var in choices)
	{
		const char* choiceName = var.first;
		CConfigNode* pChoiceConfig = pConfig->getChild(choiceName);
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

class NEURAL_NETWORK_PROBLEM_DESCRIPTION
{
protected:
	IProblem* m_pProblem;
	INetwork* m_pNetwork;

	const char* m_name;
	const char* m_comment;

public:
	NEURAL_NETWORK_PROBLEM_DESCRIPTION() = default;
	NEURAL_NETWORK_PROBLEM_DESCRIPTION(CConfigNode* pConfigNode, const char* name, const char* comment)
	{
		m_name = name;
		m_comment = comment;
		m_pProblem = CNTKWrapperLoader::getProblem(pConfigNode->FirstChildElement(m_name)->FirstChildElement("Problem"));
	}
	~NEURAL_NETWORK_PROBLEM_DESCRIPTION();

	void buildNetwork();

	INetwork* getNetwork();
	IProblem* getProblem();
};
#endif