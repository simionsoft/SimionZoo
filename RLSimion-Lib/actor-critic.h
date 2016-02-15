#pragma once

#include "actor.h"
#include "critic.h"

class CActorCritic: public CActor, public CCritic
{
	virtual void loadVFunction(const char* filename) = 0;
	virtual void saveVFunction(const char* filename) = 0;
	virtual void savePolicy(const char* pFilename)= 0;
	virtual void loadPolicy(const char* pFilename)= 0;
public:
	//CCritic interface
	virtual double updateValue(CState *s, CAction *a, CState *s_p, double r, double rho) = 0;

	//CActor interface
	virtual void selectAction(CState *s, CAction *a) = 0;

	virtual void updatePolicy(CState *s, CAction *a, CState *s_p, double r, double td){};

	virtual double getProbability(CState* s, CAction* a){ return 1.0; }

	static CActorCritic* getInstance(tinyxml2::XMLElement* pParameters);
};