#include "stdafx.h"
#include "actor-critic.h"


//The compatible features of a gaussian policy, according to
//http://www0.cs.ucl.ac.uk/staff/d.silver/web/Teaching_files/pg.pdf
//seems to be:
// \Psi_sa= \delta log pi(s,a)= (a_t-\pi(s_t)) * phi(s_t) / sigma^2

CNaturalACBhatnagar3::CNaturalACBhatnagar3(CParameters* pParameters) : CVFAActor(pParameters), CVFACritic(pParameters)
{

}

CNaturalACBhatnagar3::~CNaturalACBhatnagar3()
{}

//CActor interface
void CNaturalACBhatnagar3::updatePolicy(CState *s, CAction *a, CState *s_p, double r, double td)
{
}

//CCritic interface
double CNaturalACBhatnagar3::updateValue(CState *s, CAction *a, CState *s_p, double r, double rho)
{
	return 0.0;
}
