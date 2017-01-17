#pragma once

#include "world.h"
#include "delayed-load.h"
#include "../tools/WindowsUtils/NamedPipe.h"
#include "../tools/WindowsUtils/Process.h"
#include "../RLSimion-Lib/parameters.h"
#include "templatedConfigFile.h"

class CSetPoint;
class CRewardFunction;

class CFASTWindTurbine : public CDynamicModel, public CDeferredLoad
{
	CProcess FASTprocess,TurbSimProcess;
	CNamedPipeServer m_namedPipeServer;

	CTemplatedConfigFile m_FASTConfigTemplate, m_FASTWindConfigTemplate, m_TurbSimConfigTemplate;

	MULTI_VALUE_SIMPLE_PARAM<DOUBLE_PARAM,double> m_trainingMeanWindSpeeds;
	DOUBLE_PARAM m_evaluationMeanWindSpeed;

public:

	CFASTWindTurbine(CConfigNode* pParameters);
	virtual ~CFASTWindTurbine();

	void reset(CState *s);
	void executeAction(CState *s, const CAction *a, double dt);

	virtual void deferredLoadStep();
};
