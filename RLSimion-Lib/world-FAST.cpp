#include "stdafx.h"
#include "world-FAST.h"
#include "named-var-set.h"

#include "config.h"
#include "world.h"

#include "reward.h"


CFASTWorld::CFASTWorld(CConfigNode* pConfigNode)
{
	METADATA("World", "FAST-Wind-turbine");

	addStateVariable("T_a", "N/m", 0.0, 400000.0);
	addStateVariable("P_a", "W", 0.0, 1600000.0);
	addStateVariable("P_s", "W", 500000.0, 700000.0);
	addStateVariable("P_e", "W", 500000.0, 700000.0);
	addStateVariable("E_p", "W", -100000, 100000);
	addStateVariable("v", "m/s", 1.0, 50.0);
	addStateVariable("omega_r", "rad/s", 2.39823, 6.39823);
	addStateVariable("omega_g", "rad/s", 2.39823, 1000.39823); //what range of values does omega_g take???
	addStateVariable("E_omega_r", "rad/s", -4.0, 4.0);
	addStateVariable("d_omega_r", "rad/s^2", -2.0, 2.0);
	addStateVariable("beta", "rad", -0.3490658504, 0.5235987756);
	addStateVariable("d_beta", "rad/s", -0.1745329252, 0.1745329252);
	addStateVariable("T_g", "N/m", 100000, 162000);
	addStateVariable("d_T_g", "N/m/s", -100000, 100000);
	addStateVariable("E_int_omega_r", "rad", -100.0, 100.0);
	//action handlers
	addActionVariable("d_beta", "rad/s", -10, 10);
	addActionVariable("d_T_g", "N/m/s", -100000, 100000);

	m_pRewardFunction->addRewardComponent(new CToleranceRegionReward("v-deviation", 0.1, 1.0));
	m_pRewardFunction->initialize();

	m_namedPipeServer.openNamedPipeServer(DIMENSIONAL_PORTAL_PIPE_NAME);
}

CFASTWorld::~CFASTWorld()
{
	m_namedPipeServer.closeServer();
}



void CFASTWorld::reset(CState *s)
{
	//CreateProcess()
}

void CFASTWorld::executeAction(CState *s,const CAction *a,double dt)
{

}
