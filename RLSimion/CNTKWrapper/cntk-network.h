#pragma once

#include "../Lib/deep-network.h"

class DeepFunction;

class CntkNetwork : public IDeepNetwork
{
	DeepFunction* m_pDeepFunction; //needed to map states/actions to vectors
public:
	CntkNetwork(DeepFunction* pDeepFunction) {}
	void destroy();
	IDeepNetwork* clone(bool bFreezeWeights = true) { return nullptr; }
};

class CntkDiscreteQFunctionNetwork: public CntkNetwork
{
public:
	CntkDiscreteQFunctionNetwork();
};

class CntkContinuousQFunctionNetwork : public CntkNetwork
{

};

class CntkVFunctionNetwork : public CntkNetwork
{

};

class CntkDeterministicPolicyNetwork : public CntkNetwork
{

};