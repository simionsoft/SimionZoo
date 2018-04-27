#include "Minibatch.h"
#include "NetworkDefinition.h"

Minibatch::Minibatch(size_t size, NetworkDefinition* pNetworkDefinition, size_t outputSize)
{
	m_pNetworkDefinition = pNetworkDefinition;
	m_size = size;
	m_inputState = vector<double>(size*pNetworkDefinition->getInputStateVarIds().size());
	m_inputAction = vector<double>(size*pNetworkDefinition->getInputActionVarIds().size());

	if (outputSize == 0)
		//if not overriden, use the network's output size. This is the general case
		m_outputSize = pNetworkDefinition->getOutputSize();
	else
		m_outputSize = outputSize;

	m_output = vector<double>(size*m_outputSize);
}


Minibatch::~Minibatch()
{
}

void Minibatch::clear()
{
	m_numTuples = 0;
}

//Added for convenience to avoid vectors on the DLL client side when possible
void Minibatch::addTuple(const State* s, const Action* a, double targetValue)
{
	if (m_outputSize != 1)
		throw std::exception("Cannot use a scalar target value with multiple-output networks");

	vector<double> targetValues = vector<double>(1);
	targetValues[0] = targetValue;
	addTuple(s, a, targetValues);
}

void Minibatch::addTuple(const State* s, const Action* a, const vector<double>& targetValues)
{
	if (m_numTuples >= m_size)
		return;

	if (targetValues.size() != m_outputSize)
		throw std::exception("Missmatched tuple output size and minibatch output size");

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
	for (size_t i = 0; i < targetValues.size(); i++)
		m_output[m_numTuples*m_outputSize + i] = targetValues[i];
	
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

bool Minibatch::isFull() const
{
	return m_numTuples == m_size;
}

size_t Minibatch::size() const
{
	return m_size;
}

size_t Minibatch::outputSize() const
{
	return m_outputSize;
}