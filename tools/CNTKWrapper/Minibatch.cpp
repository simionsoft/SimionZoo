#include "Minibatch.h"
#include "NetworkDefinition.h"

Minibatch::Minibatch(size_t size, NetworkDefinition* pNetworkDefinition)
{
	m_pNetworkDefinition = pNetworkDefinition;
	m_size = size;
	m_inputState = vector<double>(size*pNetworkDefinition->getInputStateVarIds().size());
	m_inputAction = vector<double>(size*pNetworkDefinition->getInputActionVarIds().size());
	m_output = vector<double>(size*pNetworkDefinition->getOutputSize());
}


Minibatch::~Minibatch()
{
}

void Minibatch::clear()
{
	m_numTuples = 0;
}

//Added for convenience to avoid vectors when not needed
void Minibatch::addTuple(const State* s, const Action* a, double targetValue)
{
	if (m_pNetworkDefinition->getOutputSize() != 1)
		throw std::exception("Cannot use a scalar target value with multiple-output networks");

	vector<double> targetValues = vector<double>(1);
	targetValues[0] = targetValue;
	addTuple(s, a, targetValues);
}

void Minibatch::addTuple(const State* s, const Action* a, vector<double>& targetValues)
{
	if (m_numTuples >= m_size)
		return;

	//copy state input
	const vector<size_t>& stateVars= m_pNetworkDefinition->getInputStateVarIds();
	size_t stateInputSize = stateVars.size();
	for (size_t i= 0; i<stateInputSize; i++)
		m_inputState[m_numTuples*stateInputSize + i] = s->get((int) stateVars[i]);

	//copy action input
	const vector<size_t>& actionVars = m_pNetworkDefinition->getInputActionVarIds();
	size_t actionInputSize = actionVars.size();
	for (size_t i = 0; i<actionInputSize; i++)
		m_inputAction[m_numTuples*actionInputSize + i] = a->get((int) actionVars[i]);

	//copy target values
	size_t outputSize = m_pNetworkDefinition->getOutputSize();
	for (size_t i = 0; i < targetValues.size(); i++)
		m_output[m_numTuples*outputSize + i] = targetValues[i];
	
	m_numTuples++;
}

vector<double>& Minibatch::getInputState()
{
	return m_inputState;
}

vector<double>& Minibatch::getInputAction()
{
	return m_inputAction;
}

vector<double>& Minibatch::getOutput()
{
	return m_output;
}

void Minibatch::destroy()
{
	delete this;
}

bool Minibatch::isFull()
{
	return m_numTuples == m_size;
}