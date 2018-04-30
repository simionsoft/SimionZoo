#include "state-action-function.h"
#include "function-sampler.h"
#include "named-var-set.h"

FunctionSampler::FunctionSampler(string functionId, StateActionFunction* pFunction, size_t samplesPerDimension, size_t numDimensions
	, Descriptor& stateDescriptor, Descriptor& actionDescriptor)
	:m_pFunction(pFunction), m_samplesPerDimension(samplesPerDimension)

{
	m_pState = stateDescriptor.getInstance();
	m_pAction = stateDescriptor.getInstance();

	m_numOutputs = pFunction->getNumOutputs();

	m_numSamples = (size_t) pow(m_samplesPerDimension,numDimensions);
	m_sampledValues = vector<double>(m_numSamples);

	m_functionId = functionId;

	//Initialize sampling info
	//For now, we just take the first two inputs to sample values
	int stateVarIndex = 0;
	while (stateVarIndex < m_pFunction->getInputStateVariables().size()
		&& m_sampledVariableSources.size() < m_numInputs)
	{
		m_sampledVariableSources.push_back(m_pState);
		m_sampledVariableNames.push_back(m_pFunction->getInputStateVariables()[stateVarIndex]);

		++stateVarIndex;
	}

	int actionVarIndex = 0;
	while (actionVarIndex < m_pFunction->getInputActionVariables().size()
		&& m_sampledVariableSources.size() < m_numInputs)
	{
		m_sampledVariableSources.push_back(m_pAction);
		m_sampledVariableNames.push_back(m_pFunction->getInputActionVariables()[actionVarIndex]);

		++actionVarIndex;
	}

	//set all state/action variables to the its value range center
	for (int i = 0; i < m_pState->getNumVars(); ++i)
		m_pState->set(i, m_pState->getProperties(i).getMin() + m_pState->getProperties(i).getRangeWidth()*0.5);

	for (int i = 0; i < m_pAction->getNumVars(); ++i)
		m_pAction->set(i, m_pAction->getProperties(i).getMin() + m_pAction->getProperties(i).getRangeWidth()*0.5);
}

string FunctionSampler::getFunctionId() const
{
	return m_functionId;
}
size_t FunctionSampler3D::getNumSamplesX()
{
	return m_samplesPerDimension;
}
size_t FunctionSampler3D::getNumSamplesY()
{
	return m_samplesPerDimension;
}

size_t FunctionSampler::getNumOutputs() const
{
	return m_numOutputs;
}

FunctionSampler3D::FunctionSampler3D(string functionId, StateActionFunction* pFunction, size_t samplesPerDimension
	, Descriptor& stateDescriptor, Descriptor& actionDescriptor)
	:FunctionSampler(functionId, pFunction, samplesPerDimension, 2, stateDescriptor, actionDescriptor)
{
	if (m_sampledVariableNames.size() != 2)
		throw exception("FunctionSampler3D requires a function with at least two inputs");
}

const vector<double>& FunctionSampler3D::sample(unsigned int outputIndex)
{
	if (outputIndex >= m_numOutputs)
		throw exception("FunctionSampler3D::sample() was given an incorrect output index");

	double xValue, yValue;
	double xMinValue, xRangeStep, xRange;
	NamedVarProperties& xVarProperties = m_sampledVariableSources[0]->getProperties(m_sampledVariableNames[0].c_str());
	xMinValue = xVarProperties.getMin();
	xRange = xVarProperties.getRangeWidth();
	xRangeStep = xRange / (double)(m_samplesPerDimension - 1);
	double yMinValue, yRangeStep, yRange;
	NamedVarProperties& yVarProperties = m_sampledVariableSources[1]->getProperties(m_sampledVariableNames[1].c_str());
	yMinValue = yVarProperties.getMin();
	yRange = yVarProperties.getRangeWidth();
	yRangeStep = yRange / (double)(m_samplesPerDimension - 1);
	
	unsigned int i = 0;
	for (unsigned int y = 0; y < m_samplesPerDimension; ++y)
	{
		yValue = yMinValue + yRangeStep * (double)y;
		m_sampledVariableSources[1]->set(m_sampledVariableNames[1].c_str(), yValue);
		for (unsigned int x = 0; x < m_samplesPerDimension; ++x)
		{
			xValue = xMinValue + xRangeStep * (double)x;
			m_sampledVariableSources[0]->set(m_sampledVariableNames[0].c_str(), xValue);
			vector<double>& output= m_pFunction->evaluate(m_pState, m_pAction);

			m_sampledValues[i] = output[0];
			++i;
		}
	}
	return m_sampledValues;
}

FunctionSampler2D::FunctionSampler2D(string functionId, StateActionFunction* pFunction, size_t samplesPerDimension
	, Descriptor& stateDescriptor, Descriptor& actionDescriptor)
	:FunctionSampler(functionId, pFunction, samplesPerDimension, 2, stateDescriptor, actionDescriptor)
{
	if (m_sampledVariableNames.size() != 1)
		throw exception("FunctionSampler1D requires a function with at least one input");
}

const vector<double>& FunctionSampler2D::sample(unsigned int outputIndex)
{
	if (outputIndex >= m_numOutputs)
		throw exception("FunctionSampler2D::sample() was given an incorrect output index");

	double xValue, yValue;
	double xMinValue, xRangeStep, xRange;
	NamedVarProperties& xVarProperties = m_sampledVariableSources[0]->getProperties(m_sampledVariableNames[0].c_str());
	xMinValue = xVarProperties.getMin();
	xRange = xVarProperties.getRangeWidth();
	xRangeStep = xRange / (double)(m_samplesPerDimension - 1);
	double yMinValue, yRangeStep, yRange;
	NamedVarProperties& yVarProperties = m_sampledVariableSources[1]->getProperties(m_sampledVariableNames[1].c_str());
	yMinValue = yVarProperties.getMin();
	yRange = yVarProperties.getRangeWidth();
	yRangeStep = yRange / (double)(m_samplesPerDimension - 1);

	unsigned int i = 0;
	for (unsigned int x = 0; x < m_samplesPerDimension; ++x)
	{
		xValue = xMinValue + xRangeStep * (double)x;
		m_sampledVariableSources[0]->set(m_sampledVariableNames[0].c_str(), xValue);
		for (unsigned int y = 0; y < m_samplesPerDimension; ++y)
		{
			yValue = yMinValue + yRangeStep * (double)y;
			m_sampledVariableSources[1]->set(m_sampledVariableNames[1].c_str(), yValue);
			vector<double>& output = m_pFunction->evaluate(m_pState, m_pAction);

			m_sampledValues[i] = output[0];
			++i;
		}
	}
	return m_sampledValues;
}

size_t FunctionSampler2D::getNumSamplesX()
{
	return m_samplesPerDimension;
}
size_t FunctionSampler2D::getNumSamplesY()
{
	return 0;
}