#pragma once

#include <string>
#include "../../3rd-party/tinyxml2/tinyxml2.h"
#include "CNTKLibrary.h"

class CIntTuple;

using namespace std;

class CInputData
{
protected:
	string m_id;
	string m_name;
	CIntTuple* m_pShape;
	CNTK::FunctionPtr m_pInputFunctionPtr = nullptr;
	CInputData(tinyxml2::XMLElement* pNode);

public:
	CInputData(string id, CNTK::FunctionPtr pInputFunctionPtr);
	~CInputData();

	static CInputData* getInstance(tinyxml2::XMLElement* pNode);
	
	const string& getId() { return m_id; }
	const string& getName() { return m_name; }
	const CNTK::FunctionPtr getInputFunctionPtr() { return m_pInputFunctionPtr; }
	void createInputFunctionPtr();
};

