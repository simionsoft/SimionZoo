#include "stdafx.h"
#include "actor.h"
#include "parameters.h"
#include "parameter.h"
#include "noise.h"
#include "controller.h"
#include "vfa.h"
#include "features.h"
#include "world.h"
#include "globals.h"
#include "states-and-actions.h"

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

CVFAGaussianNoiseActor::CVFAGaussianNoiseActor(CParameters* pParameters)
{
	char parameterName[MAX_PARAMETER_NAME_SIZE];

	m_pOutput = g_pWorld->getActionInstance();

	m_pStateFeatures = new CFeatureList();

	m_numOutputs = (int)pParameters->getParameter("NUM_OUTPUTS")->getDouble();

	m_pAlpha = new CParameter*[m_numOutputs];
	m_pPolicy = new CFeatureVFA*[m_numOutputs];
	m_pExpNoise = new CGaussianNoise*[m_numOutputs];

	for (int i = 0; i<m_numOutputs; i++)
	{
		sprintf_s(parameterName, MAX_PARAMETER_NAME_SIZE, "LEARNING_RATE_%d", i);
		m_pAlpha[i] = pParameters->addParameter(CParameter(parameterName, 0.0));

		sprintf_s(parameterName, MAX_PARAMETER_NAME_SIZE, "POLICY_RBF_VARIABLES_%d", i);
		m_pPolicy[i] = new CRBFFeatureGridVFA(pParameters->getParameter(parameterName)->getStringPtr());

		m_pExpNoise[i] = new CGaussianNoise(i, pParameters);
	}

	if (pParameters->exists("LOAD"))
		loadPolicy(pParameters->getParameter("LOAD")->getStringPtr());

	if (pParameters->exists("SAVE"))
		strcpy_s(m_saveFilename, 1024, pParameters->getParameter("SAVE")->getStringPtr());
	else
		m_saveFilename[0] = 0;
}

CVFAGaussianNoiseActor::~CVFAGaussianNoiseActor()
{
	if (m_saveFilename[0] != 0)
		savePolicy(m_saveFilename);

	for (int i = 0; i<m_numOutputs; i++)
	{
		delete m_pPolicy[i];
		delete m_pExpNoise[i];
	}

	delete[] m_pPolicy;
	delete[] m_pExpNoise;

	if (m_pOutput)
		delete m_pOutput;

	delete m_pStateFeatures;
}