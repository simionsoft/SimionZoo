#pragma once

class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;

class CConfigNode;
class CLinearStateVFA;

class CCritic
{

protected:
	CLinearStateVFA* m_pVFunction; //value function approximator

public:
	CCritic(CConfigNode* pParameters);
	virtual ~CCritic();

	virtual double updateValue(const CState *s, const CAction *a, const CState *s_p, double r) = 0;

	static CCritic *getInstance(CConfigNode* pParameters);
};

