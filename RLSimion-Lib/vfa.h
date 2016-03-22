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

class CLinearVFA
{

protected:

	double* m_pWeights;
	unsigned int m_numWeights;

	bool m_bSaturateOutput;
	double m_minOutput, m_maxOutput;

public:
	CLinearVFA();
	virtual ~CLinearVFA();
	double getValue(const CFeatureList *features);
	double *getWeightPtr(){ return m_pWeights; }
	unsigned int getNumWeights(){ return m_numWeights; }

	void saturateOutput(double min, double max);

	bool saveWeights(const char* pFilename);
	bool loadWeights(const char* pFilename);

};

class CLinearStateVFA: public CParamObject, public CLinearVFA
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


class CLinearStateActionVFA : public CParamObject, public CLinearVFA
{
protected:

	CFeatureMap* m_pStateFeatureMap;
	CFeatureMap* m_pActionFeatureMap;
	unsigned int m_numStateWeights;
	unsigned int m_numActionWeights;

	CFeatureList *m_pAux;
	CFeatureList *m_pAux2;

public:

	CLinearStateActionVFA(CParameters* pParameters);
	~CLinearStateActionVFA();
	using CLinearVFA::getValue;
	double getValue(const CState *s, const CAction *a);

	void argMax(const CState *s, CAction* a);

	void getFeatures(const CState* s, const CAction* a, CFeatureList* outFeatures);

	//features are built using the two feature maps: the state and action feature maps
	//
	void getFeatureStateAction(unsigned int feature, CState* s, CAction* a);

	void add(const CFeatureList* pFeatures, double alpha = 1.0);

	void save(const char* pFilename);
	void load(const char* pFilename);
};
