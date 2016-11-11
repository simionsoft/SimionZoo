#pragma once
class CConfigNode;
#include <vector>
#include <initializer_list>
#include "config.h"


enum PropertyName { Comment, Default, Optional };

class CBaseParam
{
	template <typename... propertyTypes>
	void parseProperty(PropertyName propName,const char* value,propertyTypes... properties)
	{
		if (propName==Optional)
		{
			if (strcmp(value, "true") == 0) m_bOptional = true;
		}
	}
protected:
	bool m_bOptional = false;
	const char* m_comment = "No comment";

	template<typename ... propertyTypes>
	void parseProperties(PropertyName propName,const char* value,propertyTypes... properties)
	{
		parseProperty(propName,value,properties...);
	}
};

template<typename DataType>
class CSimpleParam: public CBaseParam
{

	const char* m_name;
	DataType m_value;
	void initValue(CConfigNode* pConfigNode, int& value)
	{
		value = pConfigNode->getConstInteger(m_name);
	}
	void initValue(CConfigNode* pConfigNode, double& value)
	{
		value = pConfigNode->getConstDouble(m_name);
	}
	void initValue(CConfigNode* pConfigNode, const char& value)
	{
		value = pConfigNode->getConstString(m_name);
	}
	void initValue(CConfigNode* pConfigNode, bool& value)
	{
		value = pConfigNode->getConstBoolean(m_name);
	}


public:
	CSimpleParam() = default;
	template <typename... propertyTypes>
	CSimpleParam(CConfigNode* pConfigNode,const char* name, PropertyName propName, const char* value, propertyTypes... properties)
	{
		m_name = name;
		parseProperties(propName,value,properties...);
		initValue(pConfigNode,m_value);
	}

	DataType& get() { return m_value; }
};

using INT_PARAM= CSimpleParam<int>;
using DOUBLE_PARAM = CSimpleParam<double>;
using BOOL_PARAM = CSimpleParam<bool>;
using STRING_PARAM = CSimpleParam<const char*>;
using DIR_PATH_PARAM = CSimpleParam<const char*>;
using FILE_PATH_PARAM = CSimpleParam<const char*>;

template<typename DataType>
class CHILD_CLASS: public CBaseParam
{

	const char* m_name;
	DataType* m_pValue;
public:
	CHILD_CLASS() = default;
	template <typename... propertyTypes>
	CHILD_CLASS(CConfigNode* pConfigNode, const char* name, PropertyName propName, const char* value, propertyTypes... properties)
	{
		m_name = name;
		parseProperties(propName,value,properties...);

		if (!m_bOptional || pConfigNode->getChild(name))
			m_pValue = new DataType(pConfigNode->getChild(name));
		else m_pValue = new DataType((CConfigNode*)0);
	}

	DataType& get() { return m_pValue; }
};

template<typename DataType>
class CHILD_CLASS_FACTORY : public CBaseParam
{

	const char* m_name;
	DataType* m_pValue;
public:
	CHILD_CLASS_FACTORY() = default;
	template <typename... propertyTypes>
	CHILD_CLASS_FACTORY(CConfigNode* pConfigNode, const char* name, PropertyName propName, const char* value, propertyTypes... properties)
	{
		m_name = name;
		parseProperties(propName,value,properties...);

		if (!m_bOptional || pConfigNode->getChild(name))
			m_pValue = DataType::getInstance(pConfigNode->getChild(name));
		else m_pValue = new DataType((CConfigNode*)0);
	}

	DataType& get() { return m_value; }
};

template <typename DataType>
class MULTI_VALUED: public CBaseParam
{
	const char* m_name;
	std::vector<DataType*> m_values;
public:
	MULTI_VALUED() = default;

	template<typename... propertyTypes>
	MULTI_VALUED(CConfigNode* pConfigNode,const char* name, PropertyName propName, const char* value, propertyTypes... properties)
	{
		m_name = name;
		parseProperties(propName, value, properties...);
		CConfigNode* pChildParameters = pConfigNode->getChild(name);
		while(pChildParameters!=0)
		{
			m_values.push_back(new DataType(pChildParameters));
			pChildParameters = pChildParameters->getNextSibling(name);
		}
	}
};


#include <map>
#include <functional>
template<typename DataType>
using choiceFunc= std::function<DataType*(CConfigNode*)>;

//template<typename DataType>
//using FACTORY = std::function<DataType*(CConfigNode* pConfig, const char* name, const char* comment
//						, std::map<const char*, choiceFunc<DataType>> choices)>;
//
template<typename DataType>
DataType* FACTORY_FUNC(CConfigNode* pConfig, std::map<const char*,choiceFunc<DataType>> choices)
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
		if (!pConfigNode) return 0;
		CConfigNode* pChild = pConfigNode->getChild("OPTION_NAME");
		if (pChild == 0) return 0;
		if (pChild->getChild("option1") != 0) return new testClass(pChild->getChild("option1"));
		if (pChild->getChild("option2") != 0) return new testClass(pChild->getChild("option2"));
	}
};


class test
{

	INT_PARAM intparam;
	DOUBLE_PARAM dparam;
	MULTI_VALUED<testClass> multiparam;
	CHILD_CLASS<testClass> child;
	CHILD_CLASS_FACTORY<testClass> childFac;

	static testClass* getInstance(CConfigNode* pConfigNode)
	{
		return FACTORY_FUNC<testClass>(pConfigNode, 
		{
			{"option1",[](CConfigNode* pConfigNode) {return new testClass(pConfigNode); }}
			,{ "option2",[](CConfigNode* pConfigNode) {return new testClass(pConfigNode); } }
		});
	}

public:
	test(CConfigNode* pConfigNode)
	{
		intparam = INT_PARAM(pConfigNode, "jandermor", Comment,"Nik zer dakit");
		dparam = DOUBLE_PARAM(pConfigNode, "test", Comment,"pitikan", Optional,"true", Default,"3");
		multiparam = MULTI_VALUED<testClass>(pConfigNode, "controllers", Comment,"comment=kjaral",Optional,"Visible=false");
		child = CHILD_CLASS<testClass>(pConfigNode, "my child", Comment,"his properties");
		childFac = CHILD_CLASS_FACTORY<testClass>(pConfigNode, "Factory child",Comment,"my other child from the factory");
	}

};

