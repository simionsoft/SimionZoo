#pragma once

class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;

class CParameters;


class CActor
{
protected:
	int m_numOutputs;

public:
	CActor(){}
	virtual ~CActor(){};

	virtual void selectAction(const CState *s, CAction *a)= 0;

	virtual void updatePolicy(const CState *s, const CAction *a, const CState *s_p, double r, double td){};

	virtual double getProbability(const CState* s, const CAction* a){ return 1.0; }

	static CActor *getInstance(CParameters* pParameters);
};
