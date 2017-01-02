#pragma once

#include "../../RLSimion-Lib/named-var-set.h"
#include "../../tools/WindowsUtils/NamedPipe.h"

#include <map>

#define CONTROL_DT 0.00125

class FASTStateDescriptor : public CDescriptor
{
	FASTStateDescriptor();
};

class FASTActionDescriptor : public CDescriptor
{
	FASTActionDescriptor();
};

class FASTWorldPortal
{
	CNamedPipeClient m_namedPipeClient;

	double m_lastTime;
	double m_elapsedTime;

	//constants used to calculate those variables not available at runtime
	double J_r, J_g, n_g;

	CDescriptor m_stateDescriptor, m_actionDescriptor;
	std::map<const char*, double> m_constants;
	CState *m_pS;
	CAction *m_pA;

public:
	FASTWorldPortal();

	//retrieves the state variables from the buffer
	//returns whether this is a control step or no
	bool retrieveStateVariables(float* FASTdata, bool bFirstTime);
	void sendState();

	void setActionVariables(float* FASTdata);
	void receiveAction();

	void connectToNamedPipeServer();
	void disconnectFromNamedPipeServer();
};
