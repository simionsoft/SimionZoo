/*
	SimionZoo: A framework for online model-free Reinforcement Learning on continuous
	control problems

	Copyright (c) 2016 SimionSoft. https://github.com/simionsoft

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

#include "policy.h"
#include "vfa.h"
#include "../Common/named-var-set.h"
#include "features.h"
#include "etraces.h"
#include "config.h"
#include "policy-learner.h"
#include "parameters-numeric.h"

RegularPolicyGradientLearner::RegularPolicyGradientLearner(ConfigNode* pConfigNode)
	: PolicyLearner(pConfigNode)
{
	m_pStateFeatures = new FeatureList("Actor/s");
	m_pAlpha = CHILD_OBJECT_FACTORY<NumericValue>(pConfigNode, "Alpha", "The learning gain");
}

RegularPolicyGradientLearner::~RegularPolicyGradientLearner()
{
	delete m_pStateFeatures;
}


/// <summary>
/// Updates the policies using a regular gradient-descent update rule
/// </summary>
/// <param name="s">Initial state</param>
/// <param name="a">Action</param>
/// <param name="s_p">Resultant state</param>
/// <param name="r">Reward</param>
/// <param name="behaviorProb">Probability by which the actor selected the action</param>
void RegularPolicyGradientLearner::update(const State *s, const Action *a, const State *s_p, double r, double td)
{
	double lastNoise;
	double alpha;

	//Regular gradient actor update
	//theta= theta + alpha*(lastNoise)*phi_pi(s)*td

	alpha = m_pAlpha->get();


	m_pPolicy->getFeatures(s, m_pStateFeatures);

	lastNoise = a->get(m_pPolicy->getOutputAction()) - m_pPolicy->getDeterministicOutput(m_pStateFeatures);// m_pOutput->getSample(i);


	if (alpha != 0.0)
		m_pPolicy->addFeatures(m_pStateFeatures, alpha*lastNoise*td);
}

