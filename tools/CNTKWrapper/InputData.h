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
	CIntTuple* m_pShape= nullptr;
	CNTK::Variable m_inputVariable; //TODO: Fix = nullptr
	CInputData(tinyxml2::XMLElement* pNode);
	bool m_isUsed = false; 

public:
	CInputData(string id, CNTK::Variable pInputVariable);
	~CInputData();

	static CInputData* getInstance(tinyxml2::XMLElement* pNode);
	
	const string& getId() { return m_id; }
	CNTK::NDShape getNDShape();
	const string& getName() { return m_name; }
	const CNTK::Variable getInputVariable() { return m_inputVariable; }
	void createInputVariable();

	bool getIsUsed() const { return m_isUsed; }
	void setIsUsed(bool isUsed) { m_isUsed = isUsed; }
};

