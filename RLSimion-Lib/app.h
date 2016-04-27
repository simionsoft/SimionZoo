#pragma once

class CParameters;
class CParameterFile;

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
	CParameterFile* m_pConfigDoc;
public:

	CApp();
	~CApp();

	static CApp* get();

	CLogger Logger;
	CWorld World;
	CExperiment Experiment;
	CSimGod SimGod;

	virtual void getInputFiles(CFilePathList& filePathList) = 0;
	virtual void getOutputFiles(CFilePathList& filePathList) = 0;

	virtual void run()= 0;
};
