#pragma once

class CFeatureMap;
class CFeatureList;
class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;
//CFeatureVFA////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

class CFeatureVFA
{
protected:
	CFeatureMap* m_pFeatureMap;
	double* m_pWeights;
	unsigned int m_numWeights;
public:
	virtual double getValue(CFeatureList *features)= 0;

	double *getWeightPtr(){return m_pWeights;}
	unsigned int getNumWeights(){return m_numWeights;}

	void getFeatures(CState* s,CAction* a,CFeatureList* outFeatures);
	void getFeatureStateAction(unsigned int feature, CState* s, CAction* a);
	void add(CFeatureList* pFeatures,double alpha= 1.0);

	void save(void* pFile);
	void load(void* pFile);
};

//CRBFGridVFA//////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

class CRBFFeatureGridVFA: public CFeatureVFA
{

public:
	CRBFFeatureGridVFA(char* configString);
	~CRBFFeatureGridVFA();

	double getValue(CFeatureList *features);
};