#pragma once

#include "../../../RLSimion-Lib/named-var-set.h"
#include "../../../tools/WindowsUtils/NamedPipe.h"

#include <map>


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

	double m_prevPitch, m_prevGenTorque;

	//constants used to calculate those variables not available at runtime
	double J_r, J_g, n_g;

	CDescriptor m_stateDescriptor, m_actionDescriptor;
	std::map<const char*, double> m_constants;
	CState *s;
	CAction *a;

public:
	FASTWorldPortal();

	//retrieves the state variables from the buffer
	//returns whether this is a control step or no
	void retrieveStateVariables(float* FASTdata, bool bFirstTime);
	void sendState();

	void setActionVariables(float* FASTdata, bool bFirstTime);
	void receiveAction();

	bool connectToNamedPipeServer(const char* name);
	void disconnectFromNamedPipeServer();
};
