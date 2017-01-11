#pragma once

#include "world.h"
#include "delayed-load.h"
#include "../tools/WindowsUtils/NamedPipe.h"
#include "../tools/WindowsUtils/Process.h"

class CSetPoint;
class CRewardFunction;

class CFASTWindTurbine : public CDynamicModel, public CDeferredLoad
{
	CProcess FASTprocess;
	CNamedPipeServer m_namedPipeServer;

	char* loadTemplateConfigFile(const char* templateFilename);

public:

	CFASTWindTurbine(CConfigNode* pParameters);
	virtual ~CFASTWindTurbine();

	void reset(CState *s);
	void executeAction(CState *s, const CAction *a, double dt);

	virtual void deferredLoadStep();
};
