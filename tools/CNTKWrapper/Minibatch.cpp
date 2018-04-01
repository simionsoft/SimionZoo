#include "Minibatch.h"


Minibatch::Minibatch(size_t size, INetworkDefinition* pNetworkDefinition)
{
	m_pNetworkDefinition = pNetworkDefinition;
	m_size = size;
	m_inputs = vector<double>(size*pNetworkDefinition->getNumInputStateVars());
	m_targetOutputs = vector<double>(size*pNetworkDefinition->getNumOutputs());
}


Minibatch::~Minibatch()
{
}

void Minibatch::clear()
{
	m_numTuples = 0;
}

void Minibatch::addTuple(const State* s, vector<double>& targetValues)
{
	size_t stateVarId;
	if (m_numTuples < m_size)
		return;

	for (size_t i = 0; i < m_pNetworkDefinition->getNumInputStateVars(); i++)
	{
		stateVarId = m_pNetworkDefinition->getInputStateVar(i);
		m_inputs[m_numTuples*m_pNetworkDefinition->getNumInputStateVars() + i] =
			s->get((int) stateVarId);
	}
	m_numTuples++;
}

vector<double>& Minibatch::getInputVector()
{
	return m_inputs;
}

vector<double>& Minibatch::getTargetOutputVector()
{
	return m_targetOutputs;
}

void Minibatch::destroy()
{
	delete this;
}

bool Minibatch::isFull()
{
	return m_numTuples == m_size;
}