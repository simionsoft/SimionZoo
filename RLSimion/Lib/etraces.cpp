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

#include "etraces.h"
#include "experiment.h"
#include "config.h"
#include "app.h"

ETraces::ETraces(ConfigNode* pConfigNode): FeatureList("ETraces")
{
	if (pConfigNode)
	{
		m_bUse = true;
		m_threshold = DOUBLE_PARAM(pConfigNode,"Threshold", "Threshold applied to trace factors", 0.001);
		//CONST_DOUBLE_VALUE(m_threshold,"Threshold",0.001,"Threshold applied to trace factors");
		m_lambda = DOUBLE_PARAM(pConfigNode,"Lambda", "Lambda parameter", 0.9);
		//CONST_DOUBLE_VALUE(m_lambda,"Lambda",0.9,"Lambda parameter");
		m_bReplace= BOOL_PARAM(pConfigNode,"Replace", "Replace existing traces? Or add?",true);
		//ENUM_VALUE(replace, Boolean, "Replace", "True","Replace existing traces? Or add?");
		if (m_bReplace.get()) m_overwriteMode = OverwriteMode::Replace;
		else m_overwriteMode = OverwriteMode::Add;
	}

}

ETraces::ETraces():FeatureList("ETraces")
{
	m_bUse = false;
	m_overwriteMode = OverwriteMode::Replace;
}

ETraces::~ETraces()
{}


void ETraces::update(double factor)
{
	if (!SimionApp::get()->pExperiment->isFirstStep() && m_bUse)
	{
		mult(factor* m_lambda.get());
		applyThreshold(m_threshold.get());
	}
	else
		clear();
}

void ETraces::addFeatureList(FeatureList* inList, double factor)
{
	if (m_bUse)
	{
		FeatureList::addFeatureList(inList, factor);
	}
	else
	{
		clear();
		copyMult(factor,inList);
	}
}