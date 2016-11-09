#pragma once

class CConfigNode;
class CConfigFile;

class CFilePathList;

#include "world.h"
#include "logger.h"
#include "experiment.h"
#include "simgod.h"

class CApp
{
private:
	static CApp* m_pAppInstance;
protected:
	CConfigFile* m_pConfigDoc;
public:

	CApp();
	virtual ~CApp();

	static CApp* get();

	CLogger *pLogger;
	CWorld *pWorld;
	CExperiment *pExperiment;
	CSimGod SimGod;

	virtual void getInputFiles(CFilePathList& filePathList) = 0;
	virtual void getOutputFiles(CFilePathList& filePathList) = 0;

	virtual void run()= 0;
};
