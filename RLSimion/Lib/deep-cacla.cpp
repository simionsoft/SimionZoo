#include "deep-cacla.h"



DeepCACLA::DeepCACLA(ConfigNode* pConfigNode)
{
	m_actorPolicy = CHILD_OBJECT<DeepDeterministicPolicy>(pConfigNode, "Policy", "Neural Network used to represent the actors policy");
	m_criticFunction= CHILD_OBJECT<DeepVFunction>(pConfigNode, "Value-Function", "Value function learned by the critic");
}


DeepCACLA::~DeepCACLA()
{
}
