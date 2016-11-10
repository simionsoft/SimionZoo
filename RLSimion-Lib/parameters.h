#pragma once
class CConfigNode;
#include <vector>
#include <initializer_list>

template<typename DataType>
class CLASS_PARAM
{
	const char* m_name;
	DataType m_value;
public:
	template <typename... paramType>CLASS_PARAM(const char* name, paramType... properties)
	{
		m_name = name;
	}
	virtual void init(CConfigNode* pConfigNode);
	virtual DataType& get();
};

using INT_PARAM= CLASS_PARAM<int>;
using DOUBLE_PARAM = CLASS_PARAM<double>;
using BOOL_PARAM = CLASS_PARAM<bool>;
using STRING_PARAM = CLASS_PARAM<const char*>;
using DIR_PATH_PARAM = CLASS_PARAM<const char*>;
using FILE_PATH_PARAM = CLASS_PARAM<const char*>;

INT_PARAM num_ostiya("jandermor","Comment=Nik zer dakit");
DOUBLE_PARAM valu("test","Comment=pitikan","Optional=true");

template <typename DataType>
class MULTI_VALUED
{
	const char* m_name;
	std::vector<DataType> m_values;
public:
	MULTI_VALUED(const char* name) { m_name = name; }
};
class testClass{};

MULTI_VALUED<testClass> test("haderasdlkf");

template <typename DataType>
class CHOICE_CONSTRUCTOR
{
	DataType* m_pValue;
public:
	CHOICE_CONSTRUCTOR(const char* parameterName, const char*);
};