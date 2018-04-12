#pragma once
#include <string>
#include <vector>
#include <memory>
#include "CNTKWrapper.h"
#include "../../3rd-party/tinyxml2/tinyxml2.h"

using namespace std;

class Chain;
class IParameter;

enum class LinkType
{
	InputLayer,
	ActivationLayer,
	Convolution1DLayer,
	Convolution2DLayer,
	Convolution3DLayer,
	DenseLayer,
	DropoutLayer,
	FlattenLayer,
	ReshapeLayer,
	MergeLayer,
	BatchNormalizationLayer,
	LinearTransformationLayer,
};

class Link
{
protected:
	wstring m_name;
	wstring m_id;
	vector<IParameter*> m_parameters;
	LinkType m_linkType;
	CNTK::FunctionPtr m_functionPtr = nullptr;
	Chain* m_pParentChain;

	Link(tinyxml2::XMLElement* pNode);	

public:
	Link();
	~Link();

	LinkType getLinkType() const { return m_linkType; }
	const vector<IParameter*> getParameters() const { return m_parameters; }

	const wstring& getId() const { return m_id; }
	const wstring& getName() const { return m_name; }

	const CNTK::FunctionPtr& getFunctionPtr() const { return m_functionPtr; }
	void setFunctionPtr(const CNTK::FunctionPtr& value) { m_functionPtr = value; }

	void setParentChain(Chain* pParentChain) { m_pParentChain = pParentChain; }
	const Chain* getParentChain() const { return m_pParentChain; }

	template <typename T = IParameter>
	const T* getParameterByName(const string name) const
	{
		for each (auto item in this->m_parameters)
		{
			if (item->getName()._Equal(name))
			{
				return (T*)item;
			}
		}

		return nullptr;
	}


	Link* getNextLink() const;
	Link* getPreviousLink() const;

	std::vector<const Link*> getDependencies() const;

	void createCNTKFunctionPtr();
	void createCNTKFunctionPtr(vector<const Link*> dependencies);


	static Link* Link::getInstance(tinyxml2::XMLElement* pNode);
};