#include "stdafx.h"
#include "CppUnitTest.h"
#include "../../../RLSimion/Lib/experiment.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ExperimentEpisodesSteps
{		
	TEST_CLASS(ExperimentTest)
	{
		//These tests check the functionality of the Experiment class
	public:
		
		TEST_METHOD(Experiment_Episodes)
		{
			// init
			Experiment *pExperiment = new Experiment();
			pExperiment->setEpisodeLength(2.0);
			pExperiment->setNumTrainingEpisodes(10);
			pExperiment->setEvaluationFreq(2);
			pExperiment->setNumEpisodesPerEvaluation(2); //2 subepisodes per evaluation
			pExperiment->setNumSteps(1000); //because the world hasn't been initialized, this should be used
			pExperiment->reset();

			//test
			Assert::AreEqual((unsigned int) 22,pExperiment->getTotalNumEpisodes()
				,L"getTotalNumEpisodes() not correctly calculated");
			
			pExperiment->nextStep();
			//2 evaluation episodes
			pExperiment->nextEpisode();
			Assert::AreEqual((unsigned int)1,pExperiment->getEvaluationIndex()
				,  L"getEvaluationEpisode() failed");
			Assert::AreEqual((unsigned int)1, pExperiment->getEpisodeInEvaluationIndex()
				, L"getEvaluationSubEpisodeIndex() failed");
			Assert::AreEqual(true, pExperiment->isEvaluationEpisode()
				, L"isEvaluationEpisode() failed");
			Assert::AreEqual((unsigned int)1, pExperiment->getRelativeEpisodeIndex(), L"getRelativeEpisodeIndex() failed");
			pExperiment->nextEpisode();
			Assert::AreEqual((unsigned int)1, pExperiment->getEvaluationIndex()
				, L"getEvaluationEpisode() failed");
			Assert::AreEqual((unsigned int)2, pExperiment->getEpisodeInEvaluationIndex()
				, L"getEvaluationSubEpisodeIndex() failed");
			Assert::AreEqual(true, pExperiment->isEvaluationEpisode()
				, L"isEvaluationEpisode() failed");
			Assert::AreEqual((unsigned int)1, pExperiment->getRelativeEpisodeIndex(), L"getRelativeEpisodeIndex() failed");
			//2 training episodes
			pExperiment->nextEpisode();
			Assert::AreEqual((unsigned int)1, pExperiment->getTrainingEpisodeIndex()
				, L"getEvaluationEpisode() failed");
			Assert::AreEqual(false, pExperiment->isEvaluationEpisode()
				, L"isEvaluationEpisode() failed");
			Assert::AreEqual((unsigned int)1, pExperiment->getRelativeEpisodeIndex(), L"getRelativeEpisodeIndex() failed");
			pExperiment->nextEpisode();
			Assert::AreEqual((unsigned int)2, pExperiment->getTrainingEpisodeIndex()
				, L"getEvaluationEpisode() failed");
			Assert::AreEqual(false, pExperiment->isEvaluationEpisode()
				, L"isEvaluationEpisode() failed");
			Assert::AreEqual((unsigned int)2, pExperiment->getRelativeEpisodeIndex(), L"getRelativeEpisodeIndex() failed");
			//2 evaluation episodes
			pExperiment->nextEpisode();
			Assert::AreEqual((unsigned int)2, pExperiment->getEvaluationIndex()
				, L"getEvaluationEpisode() failed");
			Assert::AreEqual((unsigned int)1, pExperiment->getEpisodeInEvaluationIndex()
				, L"getEvaluationSubEpisodeIndex() failed");
			Assert::AreEqual(true, pExperiment->isEvaluationEpisode()
				, L"isEvaluationEpisode() failed");
			Assert::AreEqual((unsigned int)2, pExperiment->getRelativeEpisodeIndex(), L"getRelativeEpisodeIndex() failed");
			pExperiment->nextEpisode();
			Assert::AreEqual((unsigned int)2, pExperiment->getEvaluationIndex()
				, L"getEvaluationEpisode() failed");
			Assert::AreEqual((unsigned int)2, pExperiment->getEpisodeInEvaluationIndex()
				, L"getEvaluationSubEpisodeIndex() failed");
			Assert::AreEqual(true, pExperiment->isEvaluationEpisode()
				, L"isEvaluationEpisode() failed");
			Assert::AreEqual((unsigned int)2, pExperiment->getRelativeEpisodeIndex(), L"getRelativeEpisodeIndex() failed");
			////2 training episodes
			pExperiment->nextEpisode();
			Assert::AreEqual((unsigned int)3, pExperiment->getTrainingEpisodeIndex()
				, L"getEvaluationEpisode() failed");
			Assert::AreEqual(false, pExperiment->isEvaluationEpisode()
				, L"isEvaluationEpisode() failed");
			Assert::AreEqual((unsigned int)3, pExperiment->getRelativeEpisodeIndex(), L"getRelativeEpisodeIndex() failed");
			pExperiment->nextEpisode();
			Assert::AreEqual((unsigned int)4, pExperiment->getTrainingEpisodeIndex()
				, L"getEvaluationEpisode() failed");
			Assert::AreEqual(false, pExperiment->isEvaluationEpisode()
				, L"isEvaluationEpisode() failed");
			Assert::AreEqual((unsigned int)4, pExperiment->getRelativeEpisodeIndex(), L"getRelativeEpisodeIndex() failed");
			delete pExperiment;
		}
		TEST_METHOD(Experiment_Episodes2)
		{
			// init
			Experiment *pExperiment = new Experiment();
			pExperiment->setEpisodeLength(2.0);
			pExperiment->setNumTrainingEpisodes(10);
			pExperiment->setEvaluationFreq(3);
			pExperiment->setNumEpisodesPerEvaluation(1); //2 subepisodes per evaluation
			pExperiment->setNumSteps(1000); //because the world hasn't been initialized, this should be used
			pExperiment->reset();

			//test
			Assert::AreEqual((unsigned int)14, pExperiment->getTotalNumEpisodes()
				, L"getTotalNumEpisodes() not correctly calculated");

			pExperiment->nextStep();
			//1 evaluation episode
			pExperiment->nextEpisode();
			Assert::AreEqual((unsigned int)1, pExperiment->getEvaluationIndex()
				, L"getEvaluationEpisode() failed");
			Assert::AreEqual((unsigned int)1, pExperiment->getEpisodeInEvaluationIndex()
				, L"getEvaluationSubEpisodeIndex() failed");
			Assert::AreEqual(true, pExperiment->isEvaluationEpisode()
				, L"isEvaluationEpisode() failed");
			Assert::AreEqual((unsigned int)1, pExperiment->getRelativeEpisodeIndex()
				, L"getRelativeEpisodeIndex() failed");
			//3 training episodes
			pExperiment->nextEpisode();
			Assert::AreEqual((unsigned int)1, pExperiment->getTrainingEpisodeIndex()
				, L"getEvaluationEpisode() failed");
			Assert::AreEqual(false, pExperiment->isEvaluationEpisode()
				, L"isEvaluationEpisode() failed");
			Assert::AreEqual((unsigned int)1, pExperiment->getRelativeEpisodeIndex()
				, L"getRelativeEpisodeIndex() failed");
			pExperiment->nextEpisode();
			Assert::AreEqual((unsigned int)2, pExperiment->getTrainingEpisodeIndex()
				, L"getEvaluationEpisode() failed");
			Assert::AreEqual(false, pExperiment->isEvaluationEpisode()
				, L"isEvaluationEpisode() failed");
			Assert::AreEqual((unsigned int)2, pExperiment->getRelativeEpisodeIndex()
				, L"getRelativeEpisodeIndex() failed");
			pExperiment->nextEpisode();
			Assert::AreEqual((unsigned int)3, pExperiment->getTrainingEpisodeIndex()
				, L"getEvaluationEpisode() failed");
			Assert::AreEqual(false, pExperiment->isEvaluationEpisode()
				, L"isEvaluationEpisode() failed");
			Assert::AreEqual((unsigned int)3, pExperiment->getRelativeEpisodeIndex()
				, L"getRelativeEpisodeIndex() failed");
			//1 evaluation episode
			pExperiment->nextEpisode();
			Assert::AreEqual((unsigned int)2, pExperiment->getEvaluationIndex()
				, L"getEvaluationEpisode() failed");
			Assert::AreEqual((unsigned int)1, pExperiment->getEpisodeInEvaluationIndex()
				, L"getEvaluationSubEpisodeIndex() failed");
			Assert::AreEqual(true, pExperiment->isEvaluationEpisode()
				, L"isEvaluationEpisode() failed");
			Assert::AreEqual((unsigned int)2, pExperiment->getRelativeEpisodeIndex()
				, L"getRelativeEpisodeIndex() failed");
			//3 training episodes
			pExperiment->nextEpisode();
			Assert::AreEqual((unsigned int)4, pExperiment->getTrainingEpisodeIndex()
				, L"getEvaluationEpisode() failed");
			Assert::AreEqual(false, pExperiment->isEvaluationEpisode()
				, L"isEvaluationEpisode() failed");
			Assert::AreEqual((unsigned int)4, pExperiment->getRelativeEpisodeIndex()
				, L"getRelativeEpisodeIndex() failed");
			pExperiment->nextEpisode();
			Assert::AreEqual((unsigned int)5, pExperiment->getTrainingEpisodeIndex()
				, L"getEvaluationEpisode() failed");
			Assert::AreEqual(false, pExperiment->isEvaluationEpisode()
				, L"isEvaluationEpisode() failed");
			Assert::AreEqual((unsigned int)5, pExperiment->getRelativeEpisodeIndex()
				, L"getRelativeEpisodeIndex() failed");
			pExperiment->nextEpisode();
			Assert::AreEqual((unsigned int)6, pExperiment->getTrainingEpisodeIndex()
				, L"getEvaluationEpisode() failed");
			Assert::AreEqual(false, pExperiment->isEvaluationEpisode()
				, L"isEvaluationEpisode() failed");
			Assert::AreEqual((unsigned int)6, pExperiment->getRelativeEpisodeIndex()
				, L"getRelativeEpisodeIndex() failed");
			delete pExperiment;
		}
		TEST_METHOD(Experiment_Progress)
		{
			// init
			Experiment *pExperiment = new Experiment();
			pExperiment->setEpisodeLength(2.0);
			pExperiment->setNumTrainingEpisodes(10);
			pExperiment->setEvaluationFreq(2);
			pExperiment->setNumEpisodesPerEvaluation(2); //2 subepisodes per evaluation
			pExperiment->setNumSteps(1000); //because the world hasn't been initialized, this should be used
			pExperiment->reset();

			//test
			if (pExperiment->getNumSteps() != 1000)
				Assert::Fail();
			//1 evaluation episode
			pExperiment->nextEpisode();
			pExperiment->nextStep();
			Assert::AreEqual(pExperiment->getTrainingProgress()
				, 0.0, L"getTrainingProgress() failed");
			pExperiment->nextEpisode();
			Assert::AreEqual(pExperiment->getTrainingProgress()
				, 0.0, L"getTrainingProgress() failed");
			//1st training episode
			pExperiment->nextEpisode();
			Assert::AreEqual(pExperiment->isEvaluationEpisode(), false, L"");
			Assert::AreEqual(pExperiment->getTrainingProgress()
				, 0.0, L"getTrainingProgress() failed");
			Assert::AreEqual(pExperiment->getTrainingEpisodeIndex(),(unsigned int) 1
				, L"getTrainingEpisodeIndex() failed");
			//2nd training episode
			pExperiment->nextEpisode();
			Assert::AreEqual(pExperiment->isEvaluationEpisode(), false, L"");
			Assert::AreEqual(pExperiment->getTrainingProgress()
				, 0.1,0.001, L"getTrainingProgress() failed");
			Assert::AreEqual(pExperiment->getTrainingEpisodeIndex(), (unsigned int)2
				, L"getTrainingEpisodeIndex() failed");
			//2nd evaluation
			pExperiment->nextEpisode();
			Assert::AreEqual(pExperiment->getTrainingProgress()
				, 0.1, 0.001, L"getTrainingProgress() failed");
			pExperiment->nextEpisode();
			Assert::AreEqual(pExperiment->getTrainingProgress()
				, 0.1, 0.001, L"getTrainingProgress() failed");
			//3rd training episode
			pExperiment->nextEpisode();
			Assert::AreEqual(pExperiment->isEvaluationEpisode(), false, L"");
			Assert::AreEqual(pExperiment->getTrainingProgress()
				, 0.2, 0.001, L"getTrainingProgress() failed");
			Assert::AreEqual(pExperiment->getTrainingEpisodeIndex(), (unsigned int)3
				, L"getTrainingEpisodeIndex() failed");
			//4th training episode
			pExperiment->nextEpisode();
			Assert::AreEqual(pExperiment->isEvaluationEpisode(), false, L"");
			Assert::AreEqual(pExperiment->getTrainingProgress()
				, 0.3, 0.001, L"getTrainingProgress() failed");
			Assert::AreEqual(pExperiment->getTrainingEpisodeIndex(), (unsigned int)4
				, L"getTrainingEpisodeIndex() failed");
			delete pExperiment;
		}

		TEST_METHOD(Experiment_OnlyOneEpisode)
		{
			// init: only one episode (evaluation) and 2 episodes per evaluation
			Experiment *pExperiment = new Experiment();
			pExperiment->setEpisodeLength(2.0);
			pExperiment->setNumTrainingEpisodes(1);
			pExperiment->setEvaluationFreq(2);
			pExperiment->setNumEpisodesPerEvaluation(2); //2 subepisodes per evaluation
			pExperiment->setNumSteps(1000); //because the world hasn't been initialized, this should be used
			pExperiment->reset();

			//test
			Assert::AreEqual((unsigned int)2, pExperiment->getTotalNumEpisodes()
				, L"getTotalNumEpisodes() not correctly calculated");
			Assert::AreEqual(pExperiment->getTotalNumEpisodes(), (unsigned int)2, L"failed");
			
			//init: only one episode (evaluation) and 1 episode per evaluation
			pExperiment->setNumEpisodesPerEvaluation(1);
			pExperiment->reset();

			Assert::AreEqual((unsigned int)1, pExperiment->getTotalNumEpisodes()
				, L"getTotalNumEpisodes() not correctly calculated");
			Assert::AreEqual(pExperiment->getTotalNumEpisodes(), (unsigned int)1, L"failed");
			delete pExperiment;
		}
	};
}