#include "stdafx.h"
#include "critic.h"
#include "vfa-critic.h"
#include "parameters.h"
#include "globals.h"

CCritic* CLASS_FACTORY(CCritic)::getInstance(CParameters* pParameters)
{
	if (!pParameters) return 0;

	const char* name = pParameters->getChild()->getName();
	CHOICE("Critic-Type");
	CHOICE_ELEMENT_FACTORY(name, "VFA-Critic", CVFACritic, pParameters->getChild());
	END_CHOICE();

	END_CLASS();
	return 0;
}
