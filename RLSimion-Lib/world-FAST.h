#pragma once

#include "world.h"
#include "app-rlsimion.h" //only needed to define MAX_PATH_SIZE
#include "delayed-load.h"
#define DIMENSIONAL_PORTAL_PIPE_NAME "FASTDimensionalPortal"
#include "../tools/WindowsUtils/NamedPipe.h"

class CSetPoint;
class CRewardFunction;

class CFASTWindTurbine : public CDynamicModel, public CDeferredLoad
{
	CNamedPipeServer m_namedPipeServer;

	char mainConfigFilename[MAX_PATH_SIZE];

	bool loadTemplateConfigFile(const char* templateFilename);
	bool saveTemplateConfigFile(const char* filename);

public:

	CFASTWindTurbine(CConfigNode* pParameters);
	virtual ~CFASTWindTurbine();

	void reset(CState *s);
	void executeAction(CState *s, const CAction *a, double dt);

	virtual void deferredLoadStep();
};
