#pragma once

class CFeatureMap;
class CFeatureList;
class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;
class CParameters;

#include "parameterized-object.h"
//CLinearVFA////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

class CLinearVFA : public CParamObject
{

protected:

	double* m_pWeights;
	unsigned int m_numWeights;

	bool m_bSaturateOutput;
	double m_minOutput, m_maxOutput;

	unsigned int m_minIndex;
	unsigned int m_maxIndex;
public:
	CLinearVFA(CParameters* pParameters);
	virtual ~CLinearVFA();
	double getValue(const CFeatureList *features);
	double *getWeightPtr(){ return m_pWeights; }
	unsigned int getNumWeights(){ return m_numWeights; }

	void saturateOutput(double min, double max);

	void setIndexOffset(unsigned int offset);

	bool saveWeights(const char* pFilename);
	bool loadWeights(const char* pFilename);

};

class CLinearStateVFA: public CLinearVFA
{
protected:
	CFeatureMap* m_pStateFeatureMap;
	CFeatureList *m_pAux;

public:
	CLinearStateVFA();
	CLinearStateVFA(CParameters* pParameters);
	virtual ~CLinearStateVFA();
	using CLinearVFA::getValue;
	double getValue(const CState *s);

	void getFeatures(const CState* s,CFeatureList* outFeatures);
	void getFeatureState(unsigned int feature, CState* s);
	void add(const CFeatureList* pFeatures,double alpha= 1.0);

	void save(const char* pFilename);
	//void load(const char* pFilename);

	static CLinearStateVFA* getInstance(CParameters* pParameters);
};

class CLinearStateVFAFromFile: public CLinearStateVFA
{
	const char* m_loadFilename;
public:
	CLinearStateVFAFromFile(CParameters* pParameters);
};


class CLinearStateActionVFA : public CLinearVFA
{
protected:

	CFeatureMap* m_pStateFeatureMap;
	CFeatureMap* m_pActionFeatureMap;
	unsigned int m_numStateWeights;
	unsigned int m_numActionWeights;

	CFeatureList *m_pAux;
	CFeatureList *m_pAux2;

public:
	unsigned int getNumStateWeights() const{ return m_numStateWeights; }
	unsigned int getNumActionWeights() const { return m_numActionWeights; }

	CLinearStateActionVFA(CParameters* pParameters);
	~CLinearStateActionVFA();
	using CLinearVFA::getValue;
	double getValue(const CState *s, const CAction *a);

	void argMax(const CState *s, CAction* a);
	double max(const CState *s);
	
	//This function fills the pre-allocated array outActionVariables with the values of the different actions in state s
	//The size of the buffer must be greater than the number of action weights
	void getActionValues(const CState* s, double *outActionValues);

	void getFeatures(const CState* s, const CAction* a, CFeatureList* outFeatures);

	//features are built using the two feature maps: the state and action feature maps
	//
	void getFeatureStateAction(unsigned int feature,CState* s, CAction* a);

	void add(const CFeatureList* pFeatures, double alpha = 1.0);

	void save(const char* pFilename) const;
	void load(const char* pFilename);
};
