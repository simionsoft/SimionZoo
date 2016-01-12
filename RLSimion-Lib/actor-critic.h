#pragma once

#include "../RLSimion-Lib/actor.h"
#include "../RLSimion-Lib/critic.h"

//Here we declare classes that implement both an actor and a critic
class CNaturalACBhatnagar3 : public CVFAActor, public CVFACritic
{
public:
	CNaturalACBhatnagar3(CParameters* pParameters);
	~CNaturalACBhatnagar3();
	//CActor interface
	void updatePolicy(CState *s, CAction *a, CState *s_p, double r, double td) ;

	//CCritic interface
	double updateValue(CState *s, CAction *a, CState *s_p, double r, double rho);
};