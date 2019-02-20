#pragma once

class StateFeatureMap;
class ActionFeatureMap;
class FeatureList;
class NamedVarSet;
typedef NamedVarSet State;
typedef NamedVarSet Action;
class ConfigNode;
class ConfigFile;

#include "parameters.h"
#include "deferred-load.h"
#include "../Common/state-action-function.h"
#include "mem-manager.h"
class IMemBuffer;


//LinearVFA////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

class LinearVFA
{
protected:
	//this is used to simplify the implementation of the StateActionFunction interface
	vector<double> m_output = vector<double>(1);

	MemManager<SimionMemPool>* m_pMemManager;
	FeatureList* m_pPendingUpdates= nullptr;
	IMemBuffer* m_pFrozenWeights = nullptr;
	IMemBuffer* m_pWeights= nullptr;
	size_t m_numWeights= 0;

	bool m_bSaturateOutput;
	double m_minOutput, m_maxOutput;

	bool m_bCanBeFrozen= false;

	size_t m_minIndex;
	size_t m_maxIndex;
public:
	LinearVFA() = default;
	LinearVFA(MemManager<SimionMemPool>* pMemManager);
	virtual ~LinearVFA();
	double get(const FeatureList *features,bool bUseFrozenWeights= true);
	IMemBuffer *getWeights(){ return m_pWeights; }
	size_t getNumWeights(){ return m_numWeights; }

	void setCanUseDeferredUpdates(bool bCanUseDeferredUpdates);
	
	void add(const FeatureList* pFeatures,double alpha= 1.0);
	void set(size_t feature, double value);

	void saturateOutput(double min, double max);

	void setIndexOffset(unsigned int offset);

};

class LinearStateVFA: public LinearVFA, public StateActionFunction, public DeferredLoad
{
protected:
	std::shared_ptr<StateFeatureMap> m_pStateFeatureMap;
	FeatureList *m_pAux;
	DOUBLE_PARAM m_initValue;
	virtual void deferredLoadStep();
public:
	LinearStateVFA() = default;
	LinearStateVFA(MemManager<SimionMemPool>* pMemManager, std::shared_ptr<StateFeatureMap> stateFeatureMap);
	LinearStateVFA(ConfigNode* pParameters);

	void setInitValue(double initValue);

	virtual ~LinearStateVFA();
	using LinearVFA::get;
	double get(const State *s);

	void getFeatures(const State* s,FeatureList* outFeatures);
	void getFeatureState(size_t feature, State* s);

	std::shared_ptr<StateFeatureMap> getStateFeatureMap(){ return m_pStateFeatureMap; }

	//StateActionFunction interface
	unsigned int getNumOutputs();
	vector<double>& evaluate(const State* s, const Action* a);
	const vector<string>& getInputStateVariables();
	const vector<string>& getInputActionVariables();
};


class LinearStateActionVFA : public LinearVFA, public StateActionFunction, public DeferredLoad
{
protected:
	std::shared_ptr<StateFeatureMap> m_pStateFeatureMap;
	std::shared_ptr<ActionFeatureMap> m_pActionFeatureMap;
	size_t m_numStateWeights;
	size_t m_numActionWeights;

	FeatureList *m_pAux = nullptr;
	FeatureList *m_pAux2 = nullptr;
	DOUBLE_PARAM m_initValue;
	int *m_pArgMaxTies= nullptr;

public:
	size_t getNumStateWeights() const{ return m_numStateWeights; }
	size_t getNumActionWeights() const { return m_numActionWeights; }
	std::shared_ptr<StateFeatureMap> getStateFeatureMap() { return m_pStateFeatureMap; }
	std::shared_ptr<ActionFeatureMap> getActionFeatureMap() { return m_pActionFeatureMap; }

	LinearStateActionVFA()= default;
	LinearStateActionVFA(ConfigNode* pParameters);
	LinearStateActionVFA(LinearStateActionVFA* pSourceVFA); //used in double q-learning to getSample a copy of the target function
	LinearStateActionVFA(MemManager<SimionMemPool>* pMemManager, std::shared_ptr<StateFeatureMap> pStateFeatureMap, std::shared_ptr<ActionFeatureMap> pActionFeatureMap);

	void setInitValue(double initValue);

	virtual ~LinearStateActionVFA();
	using LinearVFA::get;
	double get(const State *s, const Action *a);

	void argMax(const State *s, Action* a, bool bSolveTiesRandomly= false);
	double max(const State *s, bool bUseFrozenWeights= true);
	
	//This function fills the pre-allocated array outActionVariables with the values of the different actions in state s
	//The size of the buffer must be greater than the number of action weights
	void getActionValues(const State* s, double *outActionValues);

	void getFeatures(const State* s, const Action* a, FeatureList* outFeatures);

	//features are built using the two feature maps: the state and action feature maps
	//the input is a feature in state-action space
	void getFeatureStateAction(size_t feature,State* s, Action* a);

	void deferredLoadStep();

	//StateActionFunction interface
	unsigned int getNumOutputs();
	vector<double>& evaluate(const State* s, const Action* a);
	const vector<string>& getInputStateVariables();
	const vector<string>& getInputActionVariables();
};
