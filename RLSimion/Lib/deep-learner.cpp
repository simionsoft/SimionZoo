#include "deep-learner.h"


std::shared_ptr<DeepLearner> DeepLearner::getInstance(ConfigNode* pConfigNode)
{
	return CHOICE<DeepLearner>(pConfigNode, "Learner", "The learning algorithm",
	{
		{"SGD",CHOICE_ELEMENT_NEW<DeepLearnerSGD>},
		{"MomentumSGD",CHOICE_ELEMENT_NEW<DeepLearnerMomentumSGD>},
		{"Adam", CHOICE_ELEMENT_NEW<DeepLearnerAdam>}
	});
}

DeepLearner::DeepLearner(ConfigNode* pConfigNode)
{
	m_learningRate = CHILD_OBJECT_FACTORY<NumericValue>(pConfigNode, "Learning-Rate", "The learning rate used to train the Neural Network");
}

double DeepLearner::getLearningRate()
{
	return m_learningRate->get();
}


DeepLearnerSGD::DeepLearnerSGD(ConfigNode* pConfigNode)
	:DeepLearner(pConfigNode)
{}

string DeepLearnerSGD::getDefinition()
{
	return Name;
}


DeepLearnerMomentumSGD::DeepLearnerMomentumSGD(ConfigNode* pConfigNode)
	: DeepLearner(pConfigNode)
{
	m_momentum= DOUBLE_PARAM(pConfigNode, "Momentum", "Momentum parameter", 0.;
}

string DeepLearnerMomentumSGD::getDefinition()
{
	return Name;
}


DeepLearnerAdam::DeepLearnerAdam(ConfigNode* pConfigNode)
	: DeepLearner(pConfigNode)
{}

string DeepLearnerAdam::getDefinition()
{
	return Name;
}