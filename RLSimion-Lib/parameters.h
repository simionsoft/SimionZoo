#pragma once
class CConfigNode;
#include <vector>
#include <initializer_list>
#include "config.h"
#include "world.h"
#include "named-var-set.h"
#include <memory>
#include <map>
#include <functional>

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
		value = pConfigNode->getConstBoolean(m_name,m_default);
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
	void initValue(CConfigNode* pConfigNode,Distribution& value)
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
	ENUM_PARAM(CConfigNode* pConfigNode, const char* name, const char* comment,EnumType default)
	{
		m_name = name;
		m_comment = comment;
		m_default = default;
		initValue(pConfigNode,m_value);
	}
};


class STATE_VARIABLE
{
protected:
	const char* m_name;
	const char* m_comment;
	int m_hVariable= -1;
public:
	STATE_VARIABLE() = default;

	STATE_VARIABLE(CConfigNode* pConfigNode, const char* name, const char* comment)
	{
		m_hVariable = CWorld::getDynamicModel()->getStateDescriptor()
			->getVarIndex(pConfigNode->getConstString(name));
		m_name = name;
		m_comment = comment;
	}
	int get() { return m_hVariable; }
};

class ACTION_VARIABLE
{
	int m_hVariable = -1;
	const char* m_name;
	const char* m_comment; 
public:
	ACTION_VARIABLE() = default;

	ACTION_VARIABLE(CConfigNode* pConfigNode, const char* name, const char* comment)
	{
		m_hVariable = CWorld::getDynamicModel()->getActionDescriptor()
			->getVarIndex(pConfigNode->getConstString(name));
		m_name = name;
		m_comment = comment;
	}
	int get() { return m_hVariable; }
};

template<typename DataType>
class CHILD_OBJECT
{
	const char* m_name;
	const char* m_comment;
	bool m_bOptional;
	std::shared_ptr<DataType> m_pValue= 0;
public:
	CHILD_OBJECT() = default;
	CHILD_OBJECT(CConfigNode* pConfigNode, const char* name, const char* comment
		,bool bOptional=false,const char* badgerInfo="")
	{
		m_name = name;
		m_comment = comment;
		m_bOptional = bOptional;

		if (!m_bOptional || pConfigNode->getChild(name))
			m_pValue = std::shared_ptr<DataType>(new DataType(pConfigNode->getChild(name)));
		else
		{
			//we create the object without config info for default behaviour
			m_pValue = std::shared_ptr<DataType>(new DataType((CConfigNode*)nullptr));
		}
	}

	DataType* operator->() { return m_pValue.get(); }
	std::shared_ptr<DataType> shared_ptr() { return m_pValue; }
	DataType* ptr() { return m_pValue.get(); }
};

template<typename DataType>
class CHILD_OBJECT_FACTORY
{
	const char* m_name;
	const char* m_comment;
	bool m_bOptional;
	std::shared_ptr<DataType> m_pValue = 0;
public:
	CHILD_OBJECT_FACTORY() = default;
	CHILD_OBJECT_FACTORY(CConfigNode* pConfigNode, const char* name, const char* comment
		, bool bOptional= false, const char* badgerInfo= "")
	{
		m_name = name;
		m_comment = comment;
		m_bOptional = bOptional;

		if (!m_bOptional || pConfigNode->getChild(name))
			m_pValue = std::shared_ptr<DataType>(DataType::getInstance(pConfigNode->getChild(name)));
		else m_pValue = std::shared_ptr<DataType>(0);
	}

	DataType* operator->() { return m_pValue.get(); }
	std::shared_ptr<DataType> shared_ptr() { return m_pValue; }
	DataType* ptr() { return m_pValue.get(); }
};

template <typename DataType>
class MULTI_VALUE
{
protected:
	const char* m_name;
	const char* m_comment;
	bool m_bOptional;
	std::vector<std::shared_ptr<DataType>> m_values;
public:
	MULTI_VALUE() = default;

	MULTI_VALUE(CConfigNode* pConfigNode, const char* name, const char* comment, bool bOptional=false)
	{
		m_name = name;
		m_comment = comment;
		m_bOptional = bOptional;
		
		CConfigNode* pChildParameters = pConfigNode->getChild(name);
		while (pChildParameters != 0)
		{
			m_values.push_back(std::shared_ptr<DataType>(new DataType(pChildParameters)));
			pChildParameters = pChildParameters->getNextSibling(name);
		}
	}
	std::size_t size() { return m_values.size(); }
	DataType* operator[] (std::size_t index) { return m_values[index].get(); }
};

template <typename DataType>
class MULTI_VALUE_FACTORY : public MULTI_VALUE<DataType>
{
protected:
	const char* m_name;
	const char* m_comment;
	bool m_bOptional;
public:
	MULTI_VALUE_FACTORY() = default;

	MULTI_VALUE_FACTORY(CConfigNode* pConfigNode, const char* name, const char* comment, bool bOptional= false)
	{
		m_name = name;
		m_comment = comment;
		m_bOptional = bOptional;
		
		CConfigNode* pChildParameters = pConfigNode->getChild(name);
		while (pChildParameters != 0)
		{
			m_values.push_back( std::shared_ptr<DataType>(DataType::getInstance(pChildParameters)));
			pChildParameters = pChildParameters->getNextSibling(name);
		}
	}
};



template<typename DataType>
using choiceElement= std::function<std::shared_ptr<DataType>(CConfigNode*)>;

//template<typename Class>
//std::shared_ptr<Class> CHOICE_ELEMENT_NEW(CConfigNode* pConfigNode)
//{
//	return std::shared_ptr<Class>(new Class(pChildsConfig));
//}
#define CHOICE_ELEMENT_NEW(pConfigNode,className,name,comment,badgerInfo) {name,[](CConfigNode* pConfigNode) {return std::shared_ptr<className>(new className(pConfigNode->getChild(name)));}}
#define CHOICE_ELEMENT_FACTORY(pConfigNode,className,name,comment,badgerInfo) {name,[](CConfigNode* pConfigNode) {return className::getInstance(pConfigNode->getChild(name));}}

#include <list>
template<typename BaseClass>
std::shared_ptr<BaseClass> CHOICE(CConfigNode* pConfig, const char* choiceName, const char* comment
	, std::map<const char*, choiceElement<BaseClass>> choices)
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

//#define CHOICE_NEW(returnBaseClass,createdSubClass,name,comment) \
//	{name,[](CConfigNode* pChild){return std::shared_ptr<returnBaseClass>(new createdSubClass(pChild));}}

//simple tests:
//class testClass
//{
//public:
//	testClass(CConfigNode* pConfigNode)
//	{}
//
//	static std::shared_ptr<testClass> getInstance(CConfigNode* pConfigNode)
//	{
//		return CHOICE<testClass>(pConfigNode,"choice-name","explanation of its role",
//		{
//			{ "option1",[](CConfigNode* pConfigNode) {return std::shared_ptr<testClass>(new testClass(pConfigNode)); } }
//		,{ "option2",[](CConfigNode* pConfigNode) {return std::shared_ptr<testClass>(new testClass(pConfigNode)); } }
//		});
//	}
//};
//
//
//class test
//{
//
//	INT_PARAM intparam;
//	DOUBLE_PARAM dparam;
//	MULTI_VALUE<testClass> multiparam;
//	CHILD_OBJECT<testClass> child;
//	CHILD_OBJECT_FACTORY<testClass> childFac;
//
//	static std::shared_ptr<test> getInstance(CConfigNode* pConfigNode)
//	{
//		return CHOICE<test>(pConfigNode, "myname","why do you care about my name?",
//		{
//			{"option1",[](CConfigNode* pConfigNode) {return std::shared_ptr<test>(new test(pConfigNode)); }}
//			,{ "option2",[](CConfigNode* pConfigNode) {return std::shared_ptr<test>(new test(pConfigNode)); } }
//		});
//	}
//
//public:
//	test(CConfigNode* pConfigNode)
//	{
//		intparam = INT_PARAM(pConfigNode, "jandermor", "Nik zer dakit",3);
//		dparam = DOUBLE_PARAM(pConfigNode, "test", "pitikan", 3.3);
//		multiparam = MULTI_VALUE<testClass>(pConfigNode, "controllers","comment=kjaral","Visible=false");
//		child = CHILD_OBJECT<testClass>(pConfigNode, "my child", "his properties");
//		childFac = CHILD_OBJECT_FACTORY<testClass>(pConfigNode, "Factory child","my other child from the factory");
//	}
//
//};
//
