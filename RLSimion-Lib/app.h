#pragma once

class CConfigNode;
class CConfigFile;

class CFilePathList;

#include "world.h"
#include "logger.h"
#include "experiment.h"
#include "SimGod.h"
#include "parameters.h"

class CSimionApp
{
private:
	static CSimionApp* m_pAppInstance;
protected:
	CConfigFile* m_pConfigDoc;
public:

	CSimionApp();
	virtual ~CSimionApp();

	static CSimionApp* get();

	CHILD_OBJECT<CLogger> pLogger;
	CHILD_OBJECT<CWorld> pWorld;
	CHILD_OBJECT<CExperiment> pExperiment;
	CHILD_OBJECT<CSimGod> pSimGod;

	virtual void getInputFiles(CFilePathList& filePathList) = 0;
	virtual void getOutputFiles(CFilePathList& filePathList) = 0;
	virtual void setOutputDirectory(const char*) = 0;

	virtual void run()= 0;
};
