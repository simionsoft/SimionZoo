#pragma once

class CStateFeatureMap;
class CActionFeatureMap;
class CFeatureList;
class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;
class CConfigNode;
class CConfigFile;

#include "parameters.h"
#include "delayed-load.h"
#include <memory>
//CLinearVFA////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

class CLinearVFA
{

protected:

	std::shared_ptr<double> m_pWeights= nullptr;
	unsigned int m_numWeights= 0;

	bool m_bSaturateOutput;
	double m_minOutput, m_maxOutput;

	unsigned int m_minIndex;
	unsigned int m_maxIndex;
public:
	CLinearVFA() = default;
	virtual ~CLinearVFA() = default;
	double get(const CFeatureList *features);
	double *getWeightPtr(){ return m_pWeights.get(); }
	unsigned int getNumWeights(){ return m_numWeights; }

	void saturateOutput(double min, double max);

	void setIndexOffset(unsigned int offset);

	bool saveWeights(const char* pFilename) const;
	bool loadWeights(const char* pFilename);

};

class CLinearStateVFA: public CLinearVFA, public CDeferredLoad
{
protected:
	std::shared_ptr<CStateFeatureMap> m_pStateFeatureMap;
	CFeatureList *m_pAux;
	DOUBLE_PARAM m_initValue;
	virtual void deferredLoadStep();
public:
	CLinearStateVFA();
	CLinearStateVFA(CConfigNode* pParameters);
	//CLinearStateVFA(CLinearStateVFA* pSourceVFA);

	void setInitValue(double initValue);

	virtual ~CLinearStateVFA();
	using CLinearVFA::get;
	double get(const CState *s);

	void getFeatures(const CState* s,CFeatureList* outFeatures);
	void getFeatureState(unsigned int feature, CState* s);
	void add(const CFeatureList* pFeatures,double alpha= 1.0);

	void save(const char* pFilename) const;
	//void load(const char* pFilename);

	std::shared_ptr<CStateFeatureMap> getStateFeatureMap(){ return m_pStateFeatureMap; }
};


class CLinearStateActionVFA : public CLinearVFA, public CDeferredLoad
{
protected:
	std::shared_ptr<CStateFeatureMap> m_pStateFeatureMap;
	std::shared_ptr<CActionFeatureMap> m_pActionFeatureMap;
	unsigned int m_numStateWeights;
	unsigned int m_numActionWeights;

	CFeatureList *m_pAux;
	CFeatureList *m_pAux2;
	DOUBLE_PARAM m_initValue;
public:
	unsigned int getNumStateWeights() const{ return m_numStateWeights; }
	unsigned int getNumActionWeights() const { return m_numActionWeights; }
	std::shared_ptr<CStateFeatureMap> getStateFeatureMap() { return m_pStateFeatureMap; }
	std::shared_ptr<CActionFeatureMap> getActionFeatureMap() { return m_pActionFeatureMap; }

	CLinearStateActionVFA()= default;
	CLinearStateActionVFA(CConfigNode* pParameters);
	CLinearStateActionVFA(CLinearStateActionVFA* pSourceVFA); //used in double q-learning to get a copy of the target function

	void setInitValue(double initValue);

	virtual ~CLinearStateActionVFA();
	using CLinearVFA::get;
	double get(const CState *s, const CAction *a);

	void argMax(const CState *s, CAction* a);
	double max(const CState *s);
	
	//This function fills the pre-allocated array outActionVariables with the values of the different actions in state s
	//The size of the buffer must be greater than the number of action weights
	void getActionValues(const CState* s, double *outActionValues);

	void getFeatures(const CState* s, const CAction* a, CFeatureList* outFeatures);

	//features are built using the two feature maps: the state and action feature maps
	//the input is a feature in state-action space
	void getFeatureStateAction(unsigned int feature,CState* s, CAction* a);

	void add(const CFeatureList* pFeatures, double alpha = 1.0);

	void save(const char* pFilename) const;
	void load(const char* pFilename);

	void deferredLoadStep();
};
