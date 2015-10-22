#pragma once

class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;


#define MAX_FILENAME_LENGTH 512

class CLogger
{
	

public:
	CLogger(char* configFile);
	~CLogger();

	void logStep(CState *s, CAction *a,CState *s_p, double r);
};