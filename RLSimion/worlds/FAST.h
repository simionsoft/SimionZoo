#pragma once

#include "world.h"
#include "../deferred-load.h"
#include "../../tools/WindowsUtils/NamedPipe.h"
#include "../../tools/WindowsUtils/Process.h"
#include "../parameters.h"
#include "templatedConfigFile.h"

class SetPoint;
class RewardFunction;

class FASTWindTurbine : public DynamicModel, public DeferredLoad
{
	Process FASTprocess,TurbSimProcess;
	CNamedPipeServer m_namedPipeServer;

	TemplatedConfigFile m_FASTConfigTemplate, m_FASTWindConfigTemplate, m_TurbSimConfigTemplate;

	MULTI_VALUE_SIMPLE_PARAM<DOUBLE_PARAM,double> m_trainingMeanWindSpeeds;
	MULTI_VALUE_SIMPLE_PARAM<DOUBLE_PARAM, double> m_evaluationMeanWindSpeeds;

public:

	FASTWindTurbine(ConfigNode* pParameters);
	virtual ~FASTWindTurbine();

	void reset(State *s);
	void executeAction(State *s, const Action *a, double dt);

	virtual void deferredLoadStep();
};
