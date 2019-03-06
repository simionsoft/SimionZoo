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

#include "../Common/state-action-function.h"
#include "function-sampler.h"
#include "../Common/named-var-set.h"

#include <math.h>
#include <stdexcept>

FunctionSampler::~FunctionSampler()
{
	delete m_pState;
	delete m_pAction;
}

NamedVarSet* FunctionSampler::Source(VariableSource source)
{
	if (source == StateSource)
		return m_pState;
	return m_pAction;
}

FunctionSampler::FunctionSampler(string functionId, StateActionFunction* pFunction, size_t outputIndex, size_t samplesPerDimension
	, size_t numDimensions, Descriptor& stateDescriptor, Descriptor& actionDescriptor)
	: m_pFunction(pFunction), m_samplesPerDimension(samplesPerDimension)

{
	m_pState = stateDescriptor.getInstance();
	m_pAction = actionDescriptor.getInstance();

	m_outputIndex = outputIndex;
	m_numOutputs = pFunction->getNumOutputs();

	m_numSamples = (size_t) pow(m_samplesPerDimension,numDimensions);
	m_sampledValues = vector<double>(m_numSamples);

	m_functionId = functionId;

	//set all state/action variables to the its value range center
	for (int i = 0; i < (int) m_pState->getNumVars(); ++i)
		m_pState->set(i, m_pState->getDescriptor()[i].getMin() + m_pState->getDescriptor()[i].getRangeWidth()*0.5);

	for (int i = 0; i < (int) m_pAction->getNumVars(); ++i)
		m_pAction->set(i, m_pAction->getDescriptor()[i].getMin() + m_pAction->getDescriptor()[i].getRangeWidth()*0.5);
}

/// <summary>
/// Returns the name of the function
/// </summary>
string FunctionSampler3D::getFunctionId() const
{
	return m_functionId + "(" + m_xVarName + "," + m_yVarName + ")";
}

/// <summary>
/// Returns the number of samples in X (the image's width)
/// </summary>
size_t FunctionSampler3D::getNumSamplesX()
{
	return m_samplesPerDimension;
}

/// <summary>
/// Returns the number of samples in Y (the image's height)
/// </summary>
size_t FunctionSampler3D::getNumSamplesY()
{
	return m_samplesPerDimension;
}

/// <summary>
/// Returns the number of outputs of the sampler
/// </summary>
size_t FunctionSampler::getNumOutputs() const
{
	return m_numOutputs;
}

FunctionSampler3D::FunctionSampler3D(string functionId, StateActionFunction* pFunction, size_t outputIndex, size_t samplesPerDimension
	, Descriptor& stateDescriptor, Descriptor& actionDescriptor
	, VariableSource xVarSource, string xVarName, VariableSource yVarSource, string yVarName)
	:FunctionSampler(functionId, pFunction, outputIndex, samplesPerDimension, 2, stateDescriptor, actionDescriptor)
	, m_xVarName(xVarName), m_yVarName(yVarName)
{
	m_xVarSource = Source(xVarSource);
	m_yVarSource = Source(yVarSource);
}


/// <summary>
/// Samples the function and returns a vector with all the samples
/// </summary>
/// <param name="outputIndex">Index of the output (the sampler may have more than one output)</param>
/// <returns>A vector with getNumSamplesX()*getNumSamplesY() samples from the function</returns>
const vector<double>& FunctionSampler3D::sample(unsigned int outputIndex)
{
	if (outputIndex >= m_numOutputs)
		throw runtime_error("FunctionSampler3D::sample() was given an incorrect output index");

	double xValue, yValue;
	double xMinValue, xRangeStep, xRange;
	NamedVarProperties* pXProperties = m_xVarSource->getProperties(m_xVarName.c_str());// m_sampledVariableSources[0]->getProperties(m_sampledVariableNames[0].c_str());
	xMinValue = pXProperties->getMin();
	xRange = pXProperties->getRangeWidth();
	xRangeStep = xRange / (double)(m_samplesPerDimension - 1);
	double yMinValue, yRangeStep, yRange;
	NamedVarProperties* pYProperties = m_yVarSource->getProperties(m_yVarName.c_str());// m_sampledVariableSources[1]->getProperties(m_sampledVariableNames[1].c_str());
	yMinValue = pYProperties->getMin();
	yRange = pYProperties->getRangeWidth();
	yRangeStep = yRange / (double)(m_samplesPerDimension - 1);
	
	unsigned int i = 0;
	for (unsigned int y = 0; y < m_samplesPerDimension; ++y)
	{
		yValue = yMinValue + yRangeStep * (double)y;
		m_yVarSource->set(m_yVarName.c_str(), yValue);// m_sampledVariableSources[1]->set(m_sampledVariableNames[1].c_str(), yValue);
		for (unsigned int x = 0; x < m_samplesPerDimension; ++x)
		{
			xValue = xMinValue + xRangeStep * (double)x;
			m_xVarSource->set(m_xVarName.c_str(), xValue);// m_sampledVariableSources[0]->set(m_sampledVariableNames[0].c_str(), xValue);
			vector<double>& output= m_pFunction->evaluate(m_pState, m_pAction);

			m_sampledValues[i] = output[outputIndex];
			++i;
		}
	}
	return m_sampledValues;
}

FunctionSampler2D::FunctionSampler2D(string functionId, StateActionFunction* pFunction, size_t outputIndex, size_t samplesPerDimension
	, Descriptor& stateDescriptor, Descriptor& actionDescriptor
	, VariableSource xVarSource, string xVarName)
	:FunctionSampler(functionId, pFunction, outputIndex, samplesPerDimension, 1, stateDescriptor, actionDescriptor)
	, m_xVarName(xVarName)
{
	m_xVarSource = Source(xVarSource);
}


/// <summary>
/// Samples the function and returns a vector with all the samples
/// </summary>
/// <param name="outputIndex">Output index (the sampler may have more than one output)</param>
/// <returns>A vector with getNumSamplesX() samples from the function</returns>
const vector<double>& FunctionSampler2D::sample(unsigned int outputIndex)
{
	if (outputIndex >= m_numOutputs)
		throw runtime_error("FunctionSampler2D::sample() was given an incorrect output index");

	double xValue, xMinValue, xRangeStep, xRange;
	NamedVarProperties* pXProperties = m_xVarSource->getProperties(m_xVarName.c_str());
	xMinValue = pXProperties->getMin();
	xRange = pXProperties->getRangeWidth();
	xRangeStep = xRange / (double)(m_samplesPerDimension - 1);


	unsigned int i = 0;
	for (unsigned int x = 0; x < m_samplesPerDimension; ++x)
	{
		xValue = xMinValue + xRangeStep * (double)x;
		m_xVarSource->set(m_xVarName.c_str(), xValue);

		vector<double>& output = m_pFunction->evaluate(m_pState, m_pAction);

		m_sampledValues[i] = output[outputIndex];
		++i;
	}
	return m_sampledValues;
}

/// <summary>
/// Returns the name of the function
/// </summary>
string FunctionSampler2D::getFunctionId() const
{
	return m_functionId + "(" + m_xVarName + ")";
}

/// <summary>
/// Returns the number of samples in X (the image's width)
/// </summary>
size_t FunctionSampler2D::getNumSamplesX()
{
	return m_samplesPerDimension;
}

/// <summary>
/// Returns the number of samples in Y (the image's height)
/// </summary>
size_t FunctionSampler2D::getNumSamplesY()
{
	return 1;
}