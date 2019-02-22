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

#include "critic.h"
#include "vfa.h"
#include "features.h"
#include "experiment.h"
#include "vfa-critic.h"
#include "etraces.h"
#include "config.h"
#include "parameters-numeric.h"
#include "app.h"
#include "simgod.h"

TDLambdaCritic::TDLambdaCritic(ConfigNode* pConfigNode)
	: VLearnerCritic(pConfigNode)
{
	m_z = CHILD_OBJECT<ETraces>(pConfigNode, "E-Traces", "Eligibility traces of the critic", true);
	m_z->setName("Critic/E-Traces" );
	m_aux= new FeatureList("Critic/aux");
	m_pAlpha= CHILD_OBJECT_FACTORY<NumericValue>(pConfigNode,"Alpha","Learning gain", new SimpleEpisodeLinearSchedule(0.1, 0.0));
}

TDLambdaCritic::~TDLambdaCritic()
{
	delete m_aux;
}

double TDLambdaCritic::update(const State *s, const Action *a, const State *s_p, double r, double rho)
{
	double alpha = m_pAlpha->get();
	if (alpha==0.0) return 0.0;

	//using sample importance
	//z= gamma * lambda * rho * z + phi_v(s)

	double gamma = SimionApp::get()->pSimGod->getGamma();
	m_z->update(gamma*rho);

	m_pVFunction->getFeatures(s, m_aux);
	m_z->addFeatureList(m_aux,alpha);

	//theta= theta + alpha(r + gamma*v_s_p - v_s)*z
	double v_s= m_pVFunction->get(m_aux,false); //if the v-function has deferred updates, we want to getSample the actual values here

	m_pVFunction->getFeatures(s_p, m_aux);
	double v_s_p= m_pVFunction->get(m_aux);
	double td = rho*r + gamma*v_s_p - v_s;

	m_pVFunction->add(m_z.ptr(),td);

	return td;
}

