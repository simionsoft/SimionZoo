#pragma once

#include "../../RLSimion-Lib/named-var-set.h"

class FASTWorld
{
	double m_lastTime, m_last_omega_r;
	double m_T_g, m_last_T_g;

	CDescriptor m_stateDescriptor, m_actionDescriptor;
	CState *m_pS;
	CAction *m_pA;
	double m_elapsedTime;
public:
	FASTWorld();

	void retrieveStateVariables(float* FASTdata, bool bFirstTime);
	void sendState();

	void setActionVariables(float* FASTdata);
	void receiveAction();

	void connectToNamedPipeServer();
	void disconnectFromNamedPipeServer();
};
