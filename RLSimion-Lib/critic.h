#pragma once

class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;

class CParameters;
class CLinearStateVFA;

class CCritic
{
	char* m_loadFile = 0;
	char* m_saveFile = 0;
	virtual void loadVFunction(const char* filename);
	virtual void saveVFunction(const char* filename);

protected:
	CLinearStateVFA* m_pVFunction; //value function approximator

public:
	CCritic(CParameters* pParameters);
	virtual ~CCritic();

	virtual double updateValue(const CState *s, const CAction *a, const CState *s_p, double r) = 0;

	static CCritic *getInstance(CParameters* pParameters);
};

