#pragma once

#include "actor.h"
#include "critic.h"

class CActorCritic: public CActor, public CCritic
{

public:
	//CCritic interface
	virtual double updateValue(const CState *s, const CAction *a, const CState *s_p, double r, double rho) = 0;

	//CActor interface
	virtual void selectAction(const CState *s, CAction *a) = 0;

	virtual void updatePolicy(const CState *s, const CAction *a, const CState *s_p, double r, double td){};

	virtual double getProbability(const CState* s, const CAction* a){ return 1.0; }

	static CActorCritic* getInstance(CParameters* pParameters);
};