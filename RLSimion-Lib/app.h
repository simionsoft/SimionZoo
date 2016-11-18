#pragma once

class CConfigNode;
class CConfigFile;

class CFilePathList;

#include "world.h"
#include "logger.h"
#include "experiment.h"
#include "SimGod.h"

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

	CLogger *pLogger;
	CWorld *pWorld;
	CExperiment *pExperiment;
	CSimGod *pSimGod;

	virtual void getInputFiles(CFilePathList& filePathList) = 0;
	virtual void getOutputFiles(CFilePathList& filePathList) = 0;

	virtual void run()= 0;
};
