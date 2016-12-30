#pragma once

#include "world.h"
#include "../FAST/FASTDimensionalPortalDLL/FASTWorld.h"
#include "../tools/NamedPipe/NamedPipe.h"

class CSetPoint;
class CRewardFunction;

class CFASTWorld : public CDynamicModel
{
	CNamedPipeServer m_namedPipeServer;
	FASTWorld m_world;
	CSetPoint *m_pSetpoint;
public:

	CFASTWorld(CConfigNode* pParameters);
	virtual ~CFASTWorld();

	void reset(CState *s);
	void executeAction(CState *s, const CAction *a, double dt);
};
