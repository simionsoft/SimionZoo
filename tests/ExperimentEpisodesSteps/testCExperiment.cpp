#include "stdafx.h"
#include "CppUnitTest.h"
#include "../../RLSimion-Lib/experiment.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ExperimentEpisodesSteps
{		
	TEST_CLASS(CExperimentTest)
	{
	public:
		
		TEST_METHOD(TestEvaluationSubEpisodes)
		{
			// init
			CExperiment *pExperiment = new CExperiment();
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
			pExperiment->nextEpisode();
			Assert::AreEqual((unsigned int)1, pExperiment->getEvaluationIndex()
				, L"getEvaluationEpisode() failed");
			Assert::AreEqual((unsigned int)2, pExperiment->getEpisodeInEvaluationIndex()
				, L"getEvaluationSubEpisodeIndex() failed");
			Assert::AreEqual(true, pExperiment->isEvaluationEpisode()
				, L"isEvaluationEpisode() failed");
			//2 training episodes
			pExperiment->nextEpisode();
			Assert::AreEqual((unsigned int)1, pExperiment->getTrainingEpisodeIndex()
				, L"getEvaluationEpisode() failed");
			Assert::AreEqual(false, pExperiment->isEvaluationEpisode()
				, L"isEvaluationEpisode() failed");
			pExperiment->nextEpisode();
			Assert::AreEqual((unsigned int)2, pExperiment->getTrainingEpisodeIndex()
				, L"getEvaluationEpisode() failed");
			Assert::AreEqual(false, pExperiment->isEvaluationEpisode()
				, L"isEvaluationEpisode() failed");
			//2 evaluation episodes
			pExperiment->nextEpisode();
			Assert::AreEqual((unsigned int)2, pExperiment->getEvaluationIndex()
				, L"getEvaluationEpisode() failed");
			Assert::AreEqual((unsigned int)1, pExperiment->getEpisodeInEvaluationIndex()
				, L"getEvaluationSubEpisodeIndex() failed");
			Assert::AreEqual(true, pExperiment->isEvaluationEpisode()
				, L"isEvaluationEpisode() failed");
			pExperiment->nextEpisode();
			Assert::AreEqual((unsigned int)2, pExperiment->getEvaluationIndex()
				, L"getEvaluationEpisode() failed");
			Assert::AreEqual((unsigned int)2, pExperiment->getEpisodeInEvaluationIndex()
				, L"getEvaluationSubEpisodeIndex() failed");
			Assert::AreEqual(true, pExperiment->isEvaluationEpisode()
				, L"isEvaluationEpisode() failed");
			//2 training episodes
			pExperiment->nextEpisode();
			Assert::AreEqual((unsigned int)3, pExperiment->getTrainingEpisodeIndex()
				, L"getEvaluationEpisode() failed");
			Assert::AreEqual(false, pExperiment->isEvaluationEpisode()
				, L"isEvaluationEpisode() failed");
			pExperiment->nextEpisode();
			Assert::AreEqual((unsigned int)4, pExperiment->getTrainingEpisodeIndex()
				, L"getEvaluationEpisode() failed");
			Assert::AreEqual(false, pExperiment->isEvaluationEpisode()
				, L"isEvaluationEpisode() failed");
			delete pExperiment;
		}
		TEST_METHOD(TestEvaluationSubEpisodes2)
		{
			// init
			CExperiment *pExperiment = new CExperiment();
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
			//3 training episodes
			pExperiment->nextEpisode();
			Assert::AreEqual((unsigned int)1, pExperiment->getTrainingEpisodeIndex()
				, L"getEvaluationEpisode() failed");
			Assert::AreEqual(false, pExperiment->isEvaluationEpisode()
				, L"isEvaluationEpisode() failed");
			pExperiment->nextEpisode();
			Assert::AreEqual((unsigned int)2, pExperiment->getTrainingEpisodeIndex()
				, L"getEvaluationEpisode() failed");
			Assert::AreEqual(false, pExperiment->isEvaluationEpisode()
				, L"isEvaluationEpisode() failed");
			pExperiment->nextEpisode();
			Assert::AreEqual((unsigned int)3, pExperiment->getTrainingEpisodeIndex()
				, L"getEvaluationEpisode() failed");
			Assert::AreEqual(false, pExperiment->isEvaluationEpisode()
				, L"isEvaluationEpisode() failed");
			//1 evaluation episode
			pExperiment->nextEpisode();
			Assert::AreEqual((unsigned int)2, pExperiment->getEvaluationIndex()
				, L"getEvaluationEpisode() failed");
			Assert::AreEqual((unsigned int)1, pExperiment->getEpisodeInEvaluationIndex()
				, L"getEvaluationSubEpisodeIndex() failed");
			Assert::AreEqual(true, pExperiment->isEvaluationEpisode()
				, L"isEvaluationEpisode() failed");
			//3 training episodes
			pExperiment->nextEpisode();
			Assert::AreEqual((unsigned int)4, pExperiment->getTrainingEpisodeIndex()
				, L"getEvaluationEpisode() failed");
			Assert::AreEqual(false, pExperiment->isEvaluationEpisode()
				, L"isEvaluationEpisode() failed");
			pExperiment->nextEpisode();
			Assert::AreEqual((unsigned int)5, pExperiment->getTrainingEpisodeIndex()
				, L"getEvaluationEpisode() failed");
			Assert::AreEqual(false, pExperiment->isEvaluationEpisode()
				, L"isEvaluationEpisode() failed");
			pExperiment->nextEpisode();
			Assert::AreEqual((unsigned int)6, pExperiment->getTrainingEpisodeIndex()
				, L"getEvaluationEpisode() failed");
			Assert::AreEqual(false, pExperiment->isEvaluationEpisode()
				, L"isEvaluationEpisode() failed");
			delete pExperiment;
		}
	};
}