#include "stdafx.h"
#include "actor.h"
#include "parameters.h"
#include "parameter.h"
#include "controller.h"
#include "vfa.h"

CActor* CActor::m_pActor = 0;
CActor* CActor::m_pController = 0;

CActor* getNewInstance(CParameters* pParameters)
{
	const char* pAlgorithm = pParameters->getParameter("ALGORITHM")->getStringPtr();

	if (strcmp(pAlgorithm, "CACLA") == 0)
		return new CCACLAActor(pParameters);
	else if (strcmp(pAlgorithm, "VIDAL") == 0)
		return new CWindTurbineVidalController(pParameters);
	else if (strcmp(pAlgorithm, "BOUKHEZZAR") == 0)
		return new CWindTurbineBoukhezzarController(pParameters);
	else if (strcmp(pAlgorithm, "PID") == 0)
		return new CPIDController(pParameters);
	else if (strcmp(pAlgorithm, "LQR") == 0)
		return new CLQRController(pParameters);
	return 0;
}

CActor *CActor::getActorInstance(CParameters* pParameters)
{
	if (m_pActor == 0 && pParameters)
	{
		m_pActor = getNewInstance(pParameters);
	}

	return m_pActor;
}

CActor *CActor::getControllerInstance(CParameters* pParameters)
{
	if (m_pController == 0 && pParameters)
	{
		m_pController = getNewInstance(pParameters);
	}

	return m_pController;
}

