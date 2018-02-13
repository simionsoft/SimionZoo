#pragma once
#include <string>
#include <vector>
#include <memory>
#include "../../packages/CNTK.CPUOnly.2.1.0/build/native/include/CNTKLibrary.h"
#include "../../3rd-party/tinyxml2/tinyxml2.h"

using namespace std;

class CChain;
class CParameterBaseInteface;

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
};

class CLink
{
protected:
	string m_name;
	string m_id;
	vector<CParameterBaseInteface*> m_parameters;
	LinkType m_linkType;
	CNTK::FunctionPtr m_functionPtr = nullptr;
	CChain* m_pParentChain;

	CLink(tinyxml2::XMLElement* pNode);	

public:
	CLink();
	~CLink();

	LinkType getLinkType() const { return m_linkType; }
	const vector<CParameterBaseInteface*> getParameters() const { return m_parameters; }

	const string& getId() const { return m_id; }
	const string& getName() const { return m_name; }

	const CNTK::FunctionPtr& getFunctionPtr() const { return m_functionPtr; }
	void setFunctionPtr(const CNTK::FunctionPtr& value) { m_functionPtr = value; }

	void setParentChain(CChain* pParentChain) { m_pParentChain = pParentChain; }
	const CChain* getParentChain() const { return m_pParentChain; }

	template <typename T = CParameterBaseInteface>
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


	CLink* getNextLink() const;
	CLink* getPreviousLink() const;

	std::vector<const CLink*> getDependencies() const;

	void createCNTKFunctionPtr();
	void createCNTKFunctionPtr(vector<const CLink*> dependencies);


	static CLink* CLink::getInstance(tinyxml2::XMLElement* pNode);
};