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

class OptimizerParameter;

class OptimizerSettings
{
protected:
	OptimizerSettings(tinyxml2::XMLElement *pNode);
	OptimizerType m_optimizerType;
	std::vector<OptimizerParameter*> m_parameters;
public:
	OptimizerSettings();
	~OptimizerSettings();
	static OptimizerSettings* getInstance(tinyxml2::XMLElement* pNode);

	const OptimizerType& getOptimizerType() const { return m_optimizerType; }
	const std::vector<OptimizerParameter*>& getParameters() const { return m_parameters; }

	const OptimizerParameter* getParameterByKey(std::string key) const;
};

class OptimizerParameter
{
protected:
	std::string m_key;
	double m_value;

	OptimizerParameter(tinyxml2::XMLElement *pNode);

public:
	OptimizerParameter();
	~OptimizerParameter();
	static OptimizerParameter* getInstance(tinyxml2::XMLElement* pNode);

	const std::string& getKey() const { return m_key; }
	const double& getValue() const { return m_value; }
};
