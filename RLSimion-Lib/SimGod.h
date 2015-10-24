#pragma once

class CActor;
class CCritic;
class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;
class CParameters;

class CSimGod
{
	CActor* m_pActor;
	CCritic* m_pCritic;
public:

	CSimGod(CParameters* pParameters);
	~CSimGod();

	double selectAction(CState* s,CAction* a);
	double update(CState* s, CAction* a, CState* s_p, double r);
};

