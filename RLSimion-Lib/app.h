#pragma once

class CParameters;
class CParameterFile;
class CLogger;
class CWorld;
class CExperiment;
class CSimGod;
class CFilePathList;

class CApp
{
protected:
	CParameterFile* m_pConfigDoc;
public:
	static CLogger Logger;
	static CWorld World;
	static CExperiment Experiment;
	static CSimGod SimGod;

	CApp(int argc, char* argv[]);
	virtual ~CApp();

	virtual void getInputFiles(CFilePathList& filePathList) = 0;
	virtual void getOutputFiles(CFilePathList& filePathList) = 0;

	virtual void run()= 0;
};
