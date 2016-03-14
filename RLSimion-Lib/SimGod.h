#pragma once


class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;
typedef CNamedVarSet CReward;
class CParameters;
class CSimion;


class CSimGod
{
	int m_numSimions;
	CSimion** m_pSimions;
	//CActor* m_pController;
	//CActor* m_pActor;
	//CCritic* m_pCritic;

	//double m_td;

	CReward *m_pReward;
public:

	CSimGod(CParameters* pParameters);
	~CSimGod();

	void selectAction(CState* s,CAction* a);
	void update(CState* s, CAction* a, CState* s_p, double r);
};

