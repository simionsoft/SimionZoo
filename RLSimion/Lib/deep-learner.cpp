#include "deep-learner.h"
#include "config.h"

DeepLearner::DeepLearner(ConfigNode* pConfigNode)
{
	m_learningRate= CHILD_OBJECT_FACTORY<NumericValue>(pConfigNode, "Learning-Rate", "Learning rate used to train the Neural Network");
}


DeepLearner::~DeepLearner() {}

std::shared_ptr<DeepLearner> DeepLearner::getInstance(ConfigNode* pConfigNode)
{
	return CHOICE<DeepLearner>(pConfigNode, "Learner-Type", "Algorithm used to train the Neural Network",
		{
			{"SGD",CHOICE_ELEMENT_NEW<DeepLearnerSGD>},
			{"Momentum",CHOICE_ELEMENT_NEW<DeepLearnerMomentumSGD>},
			{"Adam",CHOICE_ELEMENT_NEW<DeepLearnerAdam>}
		}
	);
}

double DeepLearner::getLearningRate()
{
	return m_learningRate->get();
}

DeepLearnerSGD::DeepLearnerSGD(ConfigNode* pConfigNode) : DeepLearner(pConfigNode)
{

}

DeepLearnerMomentumSGD::DeepLearnerMomentumSGD(ConfigNode* pConfigNode) : DeepLearner(pConfigNode)
{

}

DeepLearnerAdam::DeepLearnerAdam(ConfigNode* pConfigNode) : DeepLearner(pConfigNode)
{

}