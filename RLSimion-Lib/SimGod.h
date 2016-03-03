#pragma once

class CActor;
class CCritic;
class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;
typedef CNamedVarSet CReward;
class CParameters;


class CSimGod
{
	CActor* m_pController;
	CActor* m_pActor;
	CCritic* m_pCritic;

	double m_rho;
	double m_td;

	CReward *m_pReward;
public:

	CSimGod(CParameters* pParameters);
	~CSimGod();

	double selectAction(CState* s,CAction* a);
	double update(CState* s, CAction* a, CState* s_p, double r);
};

