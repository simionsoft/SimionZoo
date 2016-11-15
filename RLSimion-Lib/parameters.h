#pragma once
class CConfigNode;
#include <vector>
#include <initializer_list>
#include "config.h"


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

template<typename DataType>
class CSimpleParam: public CBaseParam
{
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

class STATE_VARIABLE: public CBaseParam
{
	int m_hVariable= -1;
public:
	STATE_VARIABLE() = default;

	template <typename... propertyTypes>
	STATE_VARIABLE(CConfigNode* pConfigNode, const char* name, PropertyName propName, const char* value, propertyTypes... properties)
	{
		m_hVariable = CWorld::getDynamicModel()->getStateDescriptor()->getVarIndex(pParameters->getConstString(name));
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
		m_hVariable = CWorld::getDynamicModel()->getActionDescriptor()->getVarIndex(pParameters->getConstString(name));
		m_name = name;
		parseProperties(propName, value, properties...);
	}
	int get() { return m_hVariable; }
};

template<typename DataType>
class CHILD_OBJECT: public CBaseParam
{
	DataType* m_pValue= 0;
public:
	CHILD_OBJECT() = default;
	template <typename... propertyTypes>
	CHILD_OBJECT(CConfigNode* pConfigNode, const char* name, PropertyName propName, const char* value, propertyTypes... properties)
	{
		m_name = name;
		parseProperties(propName, value, properties...);

		if (!m_bOptional || pConfigNode->getChild(name))
			m_pValue = new DataType(pConfigNode->getChild(name));
		else m_pValue = new DataType((CConfigNode*)0);
	}
	
	~CHILD_OBJECT()
	{
		if (m_pValue) delete m_pValue;
	}
	DataType* operator->() { return m_pValue; }
	DataType* ptr() { return m_pValue; }
};

template<typename DataType>
class CHILD_OBJECT_FACTORY : public CBaseParam
{
	DataType* m_pValue= 0;
public:
	CHILD_OBJECT_FACTORY() = default;
	template <typename... propertyTypes>
	CHILD_OBJECT_FACTORY(CConfigNode* pConfigNode, const char* name, PropertyName propName, const char* value, propertyTypes... properties)
	{
		m_name = name;
		parseProperties(propName, value, properties...);

		if (!m_bOptional || pConfigNode->getChild(name))
			m_pValue = DataType::getInstance(pConfigNode->getChild(name));
		else m_pValue = 0;
	}
	~CHILD_OBJECT_FACTORY()
	{
		if (m_pValue) delete m_pValue;
	}

	DataType* operator->() { return m_pValue; }
	DataType* ptr() { return m_pValue; }
};

template <typename DataType>
class MULTI_VALUE: public CBaseParam
{
protected:
	std::vector<DataType*> m_values;
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
			m_values.push_back(new DataType(pChildParameters));
			pChildParameters = pChildParameters->getNextSibling(name);
		}
	}
	~MULTI_VALUE()
	{
		for (std::size_t i = 0; i < m_values.size(); i++)
			delete m_values[i];
	}
	std::size_t size() { return m_values.size(); }
	DataType* operator[] (std::size_t index) { return m_values[index]; }
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
			m_values.push_back( DataType::getInstance(pChildParameters));
			pChildParameters = pChildParameters->getNextSibling(name);
		}
	}
};


#include <map>
#include <functional>
template<typename DataType>
using choiceFunc= std::function<DataType*(CConfigNode*)>;


template<typename DataType>
DataType* FACTORY_FUNC(CConfigNode* pConfig, std::map<const char*, choiceFunc<DataType>> choices)
{
	if (!pConfig) return 0;
	for each (auto var in choices)
	{
		CConfigNode* pChoiceConfig = pConfig->getChild(var.first);
		if (pChoiceConfig != 0)
			return (DataType*)var.second(pChoiceConfig);
	}
	return 0;
}

class testClass
{
public:
	testClass(CConfigNode* pConfigNode)
	{}

	static testClass* getInstance(CConfigNode* pConfigNode)
	{
		return FACTORY_FUNC<testClass>(pConfigNode,
		{
			{ "option1",[](CConfigNode* pConfigNode) {return new testClass(pConfigNode); } }
		,{ "option2",[](CConfigNode* pConfigNode) {return new testClass(pConfigNode); } }
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

	static test* getInstance(CConfigNode* pConfigNode)
	{
		return FACTORY_FUNC<test>(pConfigNode, 
		{
			{"option1",[](CConfigNode* pConfigNode) {return new test(pConfigNode); }}
			,{ "option2",[](CConfigNode* pConfigNode) {return new test(pConfigNode); } }
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

