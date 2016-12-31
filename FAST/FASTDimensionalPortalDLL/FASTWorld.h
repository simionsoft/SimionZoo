#pragma once

#include "../../RLSimion-Lib/named-var-set.h"
#include "../../tools/WindowsUtils/NamedPipe.h"
#define DIMENSIONAL_PORTAL_PIPE_NAME "FASTDimensionalPortal"
#include <map>

class FASTStateDescriptor : public CDescriptor
{
	FASTStateDescriptor();
};

class FASTActionDescriptor : public CDescriptor
{
	FASTActionDescriptor();
};

class FASTWorld
{
	CNamedPipeClient m_namedPipeClient;

	double m_lastTime, m_last_omega_r;
	double m_T_g, m_last_T_g;
	double m_elapsedTime;

	CDescriptor m_stateDescriptor, m_actionDescriptor;
	std::map<const char*, double> m_constants;
	CState *m_pS;
	CAction *m_pA;

public:
	FASTWorld();

	void retrieveStateVariables(float* FASTdata, bool bFirstTime);
	void sendState();

	void setActionVariables(float* FASTdata);
	void receiveAction();

	void connectToNamedPipeServer();
	void disconnectFromNamedPipeServer();
};
