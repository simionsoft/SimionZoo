#pragma once

#include "world.h"
#include "delayed-load.h"
#define DIMENSIONAL_PORTAL_PIPE_NAME "FASTDimensionalPortal"
#include "../tools/WindowsUtils/NamedPipe.h"

class CSetPoint;
class CRewardFunction;

class CFASTWindTurbine : public CDynamicModel, public CDeferredLoad
{
	CNamedPipeServer m_namedPipeServer;
	//FASTWorld m_world;
	CSetPoint *m_pSetpoint;
public:

	CFASTWindTurbine(CConfigNode* pParameters);
	virtual ~CFASTWindTurbine();

	void reset(CState *s);
	void executeAction(CState *s, const CAction *a, double dt);

	virtual void deferredLoadStep();
};
