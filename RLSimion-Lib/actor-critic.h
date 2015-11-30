#pragma once

#include "../RLSimion-Lib/actor.h"
#include "../RLSimion-Lib/critic.h"

class CNaturalACBhatnagar3 : public CActor, public CCritic
{
public:
	//CActor interface
	virtual void selectAction(CState *s, CAction *a) = 0;

	virtual void updatePolicy(CState *s, CAction *a, CState *s_p, double r, double td) = 0;

	static CActor *getActorInstance(char* configFile);

	//CCritic interface
	virtual double updateValue(CState *s, CAction *a, CState *s_p, double r, double rho) = 0;

	static CCritic *getCriticInstance(char* configFile);

};