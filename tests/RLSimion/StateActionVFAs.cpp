#include "stdafx.h"
#include "CppUnitTest.h"
#include "../../RLSimion/Lib/vfa.h"
#include "../../RLSimion/Lib/app.h"
#include "../../RLSimion/Lib/features.h"
#include "../../RLSimion/Lib/simgod.h"
#include "../../RLSimion/Lib/worlds/world.h"
#include "../../RLSimion/Lib/featuremap.h"
#include "../../RLSimion/Common/named-var-set.h"
#include <iostream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace StateActionVFA
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(LinearStateActionVFA_ArgMax)
		{
			double minX = 0.0, maxX = 10.0;
			double minY = 10.0, maxY = 20.0;

			Descriptor stateDescriptor;
			size_t hX = stateDescriptor.addVariable("x", "m", minX, maxX);
			size_t hY = stateDescriptor.addVariable("y", "m", minY, maxY);
			Descriptor actionDescriptor;
			size_t hAction = actionDescriptor.addVariable("force", "N", -1.0, 1.0);

			State* s = stateDescriptor.getInstance();
			Action* a = actionDescriptor.getInstance();
			const int numFeatures = 10;

			StateFeatureMap* stateFeatureMap = new StateFeatureMap(new GaussianRBFGridFeatureMap(),stateDescriptor, { hX, hY }, numFeatures);
			ActionFeatureMap* actionFeatureMap = new ActionFeatureMap(new GaussianRBFGridFeatureMap(), actionDescriptor, { hAction }, numFeatures);

			MemManager<SimionMemPool> *pMemManager = new MemManager<SimionMemPool>();
			LinearStateActionVFA *pVFA
				= new LinearStateActionVFA(pMemManager, std::shared_ptr<StateFeatureMap>((StateFeatureMap*)stateFeatureMap)
					, std::shared_ptr<ActionFeatureMap>((ActionFeatureMap*)actionFeatureMap));

			FeatureList *outFeatures = new FeatureList("features");

			pVFA->setInitValue(0.0);
			pVFA->deferredLoadStep();
			pMemManager->deferredLoadStep();


			//Q(s= [2.5,13], a= 0.35) = 100;
			double xValue = 2.5, yValue = 13.98, maxValueAction = 0.35;
			s->set(hX, xValue);
			s->set(hY, yValue);
			for (double aValue = -1.0; aValue <= 1.0; aValue += 2. / (double)numFeatures)
			{
				a->set(hAction, aValue);
				pVFA->getFeatures(s, a, outFeatures);
				size_t mainfeature = outFeatures->maxFactorFeature();
				pVFA->set(mainfeature, -abs(maxValueAction - aValue)); // a higher value to actions closer to maxValueAction
			}
			pVFA->argMax(s, a);
			double argMax = a->get((size_t)0); // we know it's only 1 action variable, so we take the shortcut

			Assert::AreEqual(maxValueAction, argMax, 0.05, L"LinearStateActionVFA::argMax() doesn't work as expected");

			delete s;
			delete a;

			delete outFeatures;

			delete pVFA;
			delete pMemManager;
		}
		TEST_METHOD(LinearStateActionVFA_FeatureMap)
		{
			double minX = 0.0, maxX = 10.0;
			double minY = 0.0, maxY = 10.0;

			Descriptor stateDescriptor;
			size_t hX = stateDescriptor.addVariable("x", "m", minX, maxX);
			Descriptor actionDescriptor;
			size_t hAction = actionDescriptor.addVariable("force", "N", -1.0, 1.0);

			State* s = stateDescriptor.getInstance();
			State* s_p = stateDescriptor.getInstance();
			const int numFeatures = 10;

			StateFeatureMap* stateFeatureMap = new StateFeatureMap(new GaussianRBFGridFeatureMap(), stateDescriptor, { hX }, numFeatures);
			ActionFeatureMap* actionFeatureMap = new ActionFeatureMap(new GaussianRBFGridFeatureMap(), actionDescriptor, { hAction }, numFeatures);

			MemManager<SimionMemPool> *pMemManager = new MemManager<SimionMemPool>();
			LinearStateActionVFA *pVFA 
				= new LinearStateActionVFA( pMemManager, std::shared_ptr<StateFeatureMap>((StateFeatureMap*)stateFeatureMap)
											, std::shared_ptr<ActionFeatureMap>((ActionFeatureMap*)actionFeatureMap) );

			pVFA->setInitValue(0.0);
			pVFA->deferredLoadStep();
			pMemManager->deferredLoadStep();


			FeatureList *outFeatures = new FeatureList("features");

			State* pState = stateDescriptor.getInstance();
			Action* pAction = actionDescriptor.getInstance();

			State* pOutState = stateDescriptor.getInstance();
			Action* pOutAction = actionDescriptor.getInstance();

			const double x = 2.5;
			const double a = 0.8;

			pState->set(hX, x);
			pAction->set(hAction, a);
			pVFA->getFeatures(pState, pAction, outFeatures);

			double state = 0.0;
			double action = 0.0;

			for (unsigned int i = 0; i < outFeatures->m_numFeatures; i++)
			{
				pVFA->getFeatureStateAction(outFeatures->m_pFeatures[i].m_index
					,pOutState,pOutAction);
				state += outFeatures->m_pFeatures[i].m_factor*pOutState->get(hX);
				action += outFeatures->m_pFeatures[i].m_factor*pOutAction->get(hAction);
			}
			Assert::AreEqual(x, state, 0.2, L"Error doing map-unmap with LinearStateActionVFA (state)");
			Assert::AreEqual(a, action, 0.2, L"Error doing map-unmap with LinearStateActionVFA (state)");


			delete pState;
			delete pAction;
			delete pOutState;
			delete pOutAction;

			delete outFeatures;
			delete pVFA;
			delete pMemManager;
		}
	};
}