#pragma once
//#include <list>
#include "../3rd-party/tinyxml2/tinyxml2.h"
class CConfigNode;


class CConfigFile : public tinyxml2::XMLDocument
{

public:

	CConfigNode* loadFile(const char* fileName, const char* nodeName= (const char*)0);
	const char* getError();
};

//this class is a simplified interface of tinyxml2::XMLElement
class CConfigNode: public tinyxml2::XMLElement
{
	//We use downcasting, so don't add any non-static attributes here!!

public:
	CConfigNode();
	virtual ~CConfigNode();
	

	bool getConstBoolean(const char* paramName, bool defaultValue= true);
	int getConstInteger(const char* paramName, int defaultValue= 0);
	double getConstDouble(const char* paramName, double defaultValue= 0.0);
	const char* getConstString(const char* paramName= 0, const char* defaultValue= (const char*)0);

	CConfigNode* getChild(const char* paramName = (const char*)0);
	CConfigNode* getNextSibling(const char* paramName = (const char*)0);
	int countChildren(const char* paramName = 0);

	const char* getName();

	void saveFile(const char* pFilename);
	void saveFile(FILE* pFile);

	void clone(CConfigFile* pParameterFile);
};