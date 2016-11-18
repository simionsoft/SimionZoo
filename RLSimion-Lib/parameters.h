#pragma once
class CConfigNode;
#include <vector>
#include <initializer_list>
#include "config.h"
#include <memory>
#include <map>
#include <functional>

enum PropertyName { Comment, Default, Optional };

class CBaseParam
{
protected:
	template <typename... propertyTypes>
	void parseProperty(PropertyName propName, const char* value, propertyTypes... properties)
	{
		if (propName == Optional)
		{
			if (strcmp(value, "true") == 0) m_bOptional = true;
		}
	}
	bool m_bOptional = false;
	const char* m_comment = "No comment";
	const char* m_name = "Unnamed";

	template<typename ... propertyTypes>
	void parseProperties(PropertyName propName, const char* value, propertyTypes... properties)
	{
		parseProperty(propName, value, properties...);
	}
};

//Enumerated types
enum class Distribution { linear, quadratic, cubic };
enum class Interpolation { linear, quadratic, cubic };
enum class TimeReference { episode, experiment, experimentTime };

template<typename DataType>
class CSimpleParam: public CBaseParam
{
protected:
	DataType m_value;
	DataType m_default;
	
	void initValue(CConfigNode* pConfigNode, int& value)
	{
		value = pConfigNode->getConstInteger(m_name,m_default);
	}
	void initValue(CConfigNode* pConfigNode, double& value)
	{
		value = pConfigNode->getConstDouble(m_name,m_default);
	}
	void initValue(CConfigNode* pConfigNode, const char& value)
	{
		value = pConfigNode->getConstString(m_name,m_default);
	}
	void initValue(CConfigNode* pConfigNode, bool& value)
	{
		value = pConfigNode->getConstBoolean(m_name);
	}
	void initValue(CConfigNode* pConfigNode, TimeReference& value)
	{
		const char* strValue = pConfigNode->getConstString(m_name);
		if (!strcmp(strValue, "episode")) value = TimeReference::episode;
		else if (!strcmp(strValue, "experiment")) value = TimeReference::experiment;
		else if (!strcmp(strValue, "experimentTime")) value = TimeReference::experimentTime;
	}
	void initValue(CConfigNode* pConfigNode,Distribution& value)
	{
		const char* strValue = pConfigNode->getConstString(m_name);
		if (!strcmp(strValue, "linear")) value = Distribution::linear;
		else if (!strcmp(strValue, "quadratic")) value = Distribution::quadratic;
		else if (!strcmp(strValue, "cubic")) value = Distribution::cubic;
	}
	void initValue(CConfigNode* pConfigNode, Interpolation& value)
	{
		const char* strValue = pConfigNode->getConstString(m_name);
		if (!strcmp(strValue, "linear")) value = Interpolation::linear;
		else if (!strcmp(strValue, "quadratic")) value = Interpolation::quadratic;
		else if (!strcmp(strValue, "cubic")) value = Interpolation::cubic;
	}
public:
	CSimpleParam() = default;
	template <typename... propertyTypes> CSimpleParam(CConfigNode* pConfigNode, const char* name, PropertyName propName
		, const char* value, propertyTypes... properties)
	{
		m_name = name;
		parseProperties(propName, value, properties...);
		initValue(pConfigNode, m_value);
	}

	DataType& get() { return m_value; }
};

using INT_PARAM= CSimpleParam<int>;
using DOUBLE_PARAM = CSimpleParam<double>;
using BOOL_PARAM = CSimpleParam<bool>;
using STRING_PARAM = CSimpleParam<const char*>;
using DIR_PATH_PARAM = CSimpleParam<const char*>;
using FILE_PATH_PARAM = CSimpleParam<const char*>;


template <typename EnumType>
class ENUM_PARAM: public CSimpleParam<EnumType>
{
public:
	ENUM_PARAM() = default;
	template <typename... propertyTypes>
	ENUM_PARAM(CConfigNode* pConfigNode, const char* name, PropertyName propName, const char* value, propertyTypes... properties)
	{
		m_name = name;
		parseProperties(propName, value, properties...);
		initValue(pConfigNode,m_value);
	}
};


class STATE_VARIABLE: public CBaseParam
{
	int m_hVariable= -1;
public:
	STATE_VARIABLE() = default;

	template <typename... propertyTypes>
	STATE_VARIABLE(CConfigNode* pConfigNode, const char* name, PropertyName propName, const char* value, propertyTypes... properties)
	{
		m_hVariable = CWorld::getDynamicModel()->getStateDescriptor()
			->getVarIndex(pConfigNode->getConstString(name));
		m_name = name;
		parseProperties(propName, value, properties...);
	}
	int get() { return m_hVariable; }
};

class ACTION_VARIABLE : public CBaseParam
{
	int m_hVariable = -1;
public:
	ACTION_VARIABLE() = default;

	template <typename... propertyTypes>
	ACTION_VARIABLE(CConfigNode* pConfigNode, const char* name, PropertyName propName, const char* value, propertyTypes... properties)
	{
		m_hVariable = CWorld::getDynamicModel()->getActionDescriptor()
			->getVarIndex(pConfigNode->getConstString(name));
		m_name = name;
		parseProperties(propName, value, properties...);
	}
	int get() { return m_hVariable; }
};

template<typename DataType>
class CHILD_OBJECT: public CBaseParam
{
	std::shared_ptr<DataType> m_pValue= 0;
public:
	CHILD_OBJECT() = default;
	template <typename... propertyTypes>
	CHILD_OBJECT(CConfigNode* pConfigNode, const char* name, PropertyName propName, const char* value, propertyTypes... properties)
	{
		m_name = name;
		parseProperties(propName, value, properties...);

		if (!m_bOptional || pConfigNode->getChild(name))
			m_pValue = std::shared_ptr<DataType>(new DataType(pConfigNode->getChild(name)));
		else m_pValue = std::shared_ptr<DataType>(0);
	}
	
	~CHILD_OBJECT() {}
	DataType* operator->() { return m_pValue.get(); }
	std::shared_ptr<DataType> ptr() { return m_pValue; }
};

template<typename DataType>
class CHILD_OBJECT_FACTORY : public CBaseParam
{
	std::shared_ptr<DataType> m_pValue = 0;
public:
	CHILD_OBJECT_FACTORY() = default;
	template <typename... propertyTypes>
	CHILD_OBJECT_FACTORY(CConfigNode* pConfigNode, const char* name, PropertyName propName, const char* value, propertyTypes... properties)
	{
		m_name = name;
		parseProperties(propName, value, properties...);

		if (!m_bOptional || pConfigNode->getChild(name))
			m_pValue = std::shared_ptr<DataType>(DataType::getInstance(pConfigNode->getChild(name)));
		else m_pValue = std::shared_ptr<DataType>(0);
	}
	~CHILD_OBJECT_FACTORY()
	{}

	DataType* operator->() { return m_pValue.get(); }
	std::shared_ptr<DataType> ptr() { return m_pValue; }
};

template <typename DataType>
class MULTI_VALUE: public CBaseParam
{
protected:
	std::vector<std::shared_ptr<DataType>> m_values;
public:
	MULTI_VALUE() = default;

	template<typename... propertyTypes>
	MULTI_VALUE(CConfigNode* pConfigNode, const char* name, PropertyName propName
		, const char* value, propertyTypes... properties)
	{
		m_name = name;
		parseProperties(propName, value, properties...);
		CConfigNode* pChildParameters = pConfigNode->getChild(name);
		while (pChildParameters != 0)
		{
			m_values.push_back(std::shared_ptr<DataType>(new DataType(pChildParameters)));
			pChildParameters = pChildParameters->getNextSibling(name);
		}
	}
	~MULTI_VALUE() {}
	std::size_t size() { return m_values.size(); }
	DataType* operator[] (std::size_t index) { return m_values[index].get(); }
};

template <typename DataType>
class MULTI_VALUE_FACTORY : public MULTI_VALUE<DataType>
{
public:
	MULTI_VALUE_FACTORY() = default;
	template<typename... propertyTypes>
	MULTI_VALUE_FACTORY(CConfigNode* pConfigNode, const char* name, PropertyName propName
		, const char* value, propertyTypes... properties)
	{
		m_name = name;
		parseProperties(propName, value, properties...);
		CConfigNode* pChildParameters = pConfigNode->getChild(name);
		while (pChildParameters != 0)
		{
			m_values.push_back( std::shared_ptr<DataType>(DataType::getInstance(pChildParameters)));
			pChildParameters = pChildParameters->getNextSibling(name);
		}
	}
};



template<typename DataType>
using choiceFunc= std::function<std::shared_ptr<DataType>(CConfigNode*)>;


template<typename DataType>
std::shared_ptr<DataType> CHOICE_FUNC(CConfigNode* pConfig, const char* choiceName, const char* comment
	, std::map<const char*, choiceFunc<DataType>> choices)
{
	if (!pConfig) return 0;
	pConfig = pConfig->getChild(choiceName);
	if (!pConfig) return 0;

	for each (auto var in choices)
	{
		CConfigNode* pChoiceConfig = pConfig->getChild(var.first);
		if (pChoiceConfig != 0)
			return var.second(pChoiceConfig);
	}
	return 0;
}

class testClass
{
public:
	testClass(CConfigNode* pConfigNode)
	{}

	static std::shared_ptr<testClass> getInstance(CConfigNode* pConfigNode)
	{
		return CHOICE_FUNC<testClass>(pConfigNode,"choice-name","explanation of its role",
		{
			{ "option1",[](CConfigNode* pConfigNode) {return std::shared_ptr<testClass>(new testClass(pConfigNode)); } }
		,{ "option2",[](CConfigNode* pConfigNode) {return std::shared_ptr<testClass>(new testClass(pConfigNode)); } }
		});
	}
};


class test
{

	INT_PARAM intparam;
	DOUBLE_PARAM dparam;
	MULTI_VALUE<testClass> multiparam;
	CHILD_OBJECT<testClass> child;
	CHILD_OBJECT_FACTORY<testClass> childFac;

	static std::shared_ptr<test> getInstance(CConfigNode* pConfigNode)
	{
		return CHOICE_FUNC<test>(pConfigNode, "myname","why do you care about my name?",
		{
			{"option1",[](CConfigNode* pConfigNode) {return std::shared_ptr<test>(new test(pConfigNode)); }}
			,{ "option2",[](CConfigNode* pConfigNode) {return std::shared_ptr<test>(new test(pConfigNode)); } }
		});
	}

public:
	test(CConfigNode* pConfigNode)
	{
		intparam = INT_PARAM(pConfigNode, "jandermor", Comment,"Nik zer dakit");
		dparam = DOUBLE_PARAM(pConfigNode, "test", Comment,"pitikan", Optional,"true", Default,"3");
		multiparam = MULTI_VALUE<testClass>(pConfigNode, "controllers", Comment,"comment=kjaral",Optional,"Visible=false");
		child = CHILD_OBJECT<testClass>(pConfigNode, "my child", Comment,"his properties");
		childFac = CHILD_OBJECT_FACTORY<testClass>(pConfigNode, "Factory child",Comment,"my other child from the factory");
	}

};

