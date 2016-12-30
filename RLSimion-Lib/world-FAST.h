#pragma once

#include "world.h"
#include "../FAST/FASTDimensionalPortalDLL/FASTWorld.h"
#include "../tools/WindowsUtils/NamedPipe.h"

class CSetPoint;
class CRewardFunction;

class CFASTWindTurbine : public CDynamicModel
{
	CNamedPipeServer m_namedPipeServer;
	//FASTWorld m_world;
	CSetPoint *m_pSetpoint;
public:

	CFASTWindTurbine(CConfigNode* pParameters);
	virtual ~CFASTWindTurbine();

	void reset(CState *s);
	void executeAction(CState *s, const CAction *a, double dt);
};
