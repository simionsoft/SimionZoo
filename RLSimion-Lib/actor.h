#pragma once

class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;

class CParameters;
class CPolicyLearner;

class CActor
{
	CPolicyLearner **m_pPolicyLearners;
protected:
	int m_numOutputs;
	char* m_loadFile = 0;
	char* m_saveFile = 0;
public:
	CActor(CParameters* pParameters);
	virtual ~CActor();

	virtual void selectAction(const CState *s, CAction *a);

	virtual void updatePolicy(const CState *s, const CAction *a, const CState *s_p, double r, double td);


	void savePolicy(const char* pFilename);
	void loadPolicy(const char* pFilename);
};
