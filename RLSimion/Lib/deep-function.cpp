#include "deep-function.h"


DeepFunction::DeepFunction(ConfigNode* pConfigNode)
{
	m_layers= MULTI_VALUE<DeepLayer>(pConfigNode, "Layers", "Layers of the Neural Network");
	m_learner = CHILD_OBJECT_FACTORY<DeepLearner>(pConfigNode, "Learner", "Learner used for this Neural Network");
	m_useMinibatchNormalization = BOOL_PARAM(pConfigNode, "Use-Normalization", "Use minibatch normalization", false);
	m_minibatchSize = INT_PARAM(pConfigNode, "Minibatch-Size", "Number of tuples in each minibatch used in updates", 100);
}

void DeepFunction::DeferredLoadStep()
{
	//TODO: initalize deep function
}