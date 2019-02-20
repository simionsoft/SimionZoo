#pragma once

#include "../../../RLSimion/Common/named-var-set.h"
#include "../../../tools/System/NamedPipe.h"

#include <map>


class FASTStateDescriptor : public Descriptor
{
	FASTStateDescriptor();
};

class FASTActionDescriptor : public Descriptor
{
	FASTActionDescriptor();
};

class FASTWorldPortal
{
	NamedPipeClient m_namedPipeClient;

	double m_lastTime;
	double m_elapsedTime;

	double m_prevPitch, m_prevGenTorque;

	//constants used to calculate those variables not available at runtime
	double J_r, J_g, n_g;

	Descriptor m_stateDescriptor, m_actionDescriptor;
	std::map<const char*, double> m_constants;
	State *s;
	Action *a;

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
