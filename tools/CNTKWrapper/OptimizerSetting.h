#pragma once
#include "../../3rd-party/tinyxml2/tinyxml2.h"
#include <vector>

enum class OptimizerType
{
	SGD,
	MomentumSGD,
	Nesterov,
	FSAdaGrad,
	Adam,
	AdaGrad,
	RMSProp,
	AdaDelta,
};

class COptimizerParameter;

class OptimizerSettings
{
protected:
	OptimizerSettings(tinyxml2::XMLElement *pNode);
	OptimizerType m_optimizerType;
	std::vector<COptimizerParameter*> m_parameters;
public:
	OptimizerSettings();
	~OptimizerSettings();
	static OptimizerSettings* getInstance(tinyxml2::XMLElement* pNode);

	const OptimizerType& getOptimizerType() const { return m_optimizerType; }
	const std::vector<COptimizerParameter*>& getParameteres() const { return m_parameters; }

	const COptimizerParameter* getParameterByKey(std::string key) const;
};

class COptimizerParameter
{
protected:
	std::string m_key;
	double m_value;

	COptimizerParameter(tinyxml2::XMLElement *pNode);

public:
	COptimizerParameter();
	~COptimizerParameter();
	static COptimizerParameter* getInstance(tinyxml2::XMLElement* pNode);

	const std::string& getKey() const { return m_key; }
	const double& getValue() const { return m_value; }
};
