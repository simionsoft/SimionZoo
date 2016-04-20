#pragma once


class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;
typedef CNamedVarSet CReward;
class CParameters;
class CSimion;
class CDeferredLoad;

#include <vector>


class CSimGod
{
	int m_numSimions= 0;
	CSimion** m_pSimions = 0;

	CReward *m_pReward;
	std::vector<CDeferredLoad*> m_delayedLoadObjects;
public:
	CSimGod();
	~CSimGod();

	void init(CParameters* pParameters);

	void selectAction(CState* s,CAction* a);
	void update(CState* s, CAction* a, CState* s_p, double r);

	void registerDelayedLoadObj(CDeferredLoad* pObj);
	void delayedLoad();
};

