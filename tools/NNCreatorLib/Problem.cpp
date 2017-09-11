#include "stdafx.h"
#include "Problem.h"
#include "xmltags.h"
#include "InputData.h"
#include "NetworkArchitecture.h"
#include "Link.h"
#include "Chain.h"
#include "ParameterValues.h"
#include "Network.h"
#include "OptimizerSetting.h"
#include "Exceptions.h"

CProblem::CProblem(tinyxml2::XMLElement * pParentNode)
{
	//load architecture
	tinyxml2::XMLElement *pNode = pParentNode->FirstChildElement(XML_TAG_NETWORK_ARCHITECTURE);
	if (pNode == nullptr)
		throw ProblemParserElementNotFound(XML_TAG_NETWORK_ARCHITECTURE);

	m_pNetworkArchitecture = CNetworkArchitecture::getInstance(pNode);
	m_pNetworkArchitecture->setParentProblem(this);

	//load inputs
	pNode = pParentNode->FirstChildElement(XML_TAG_Inputs);
	if (pNode == nullptr)
		throw ProblemParserElementNotFound(XML_TAG_Inputs);
	loadChildren<CInputData>(pNode, XML_TAG_InputData, m_inputs);

	//load output
	pNode = pParentNode->FirstChildElement(XML_TAG_Output);
	if (pNode == nullptr)
		throw ProblemParserElementNotFound(XML_TAG_Output);
	pNode = pNode->FirstChildElement(XML_TAG_LinkConnection);
	if (pNode == nullptr)
		throw ProblemParserElementNotFound(XML_TAG_LinkConnection);
	m_pOutput = CLinkConnection::getInstance(pNode);

	//load optimizer
	pNode = pParentNode->FirstChildElement(XML_TAG_OptimizerSetting)->FirstChildElement(XML_TAG_Optimizer);
	if (pNode == nullptr)
		throw ProblemParserElementNotFound(XML_TAG_Optimizer);
	m_pOptimizerSetting = COptimizerSetting::getInstance(pNode);
}

CProblem::CProblem()
{
}

CProblem::~CProblem()
{
}

CProblem * CProblem::getInstance(tinyxml2::XMLElement * pNode)
{
	if (!strcmp(pNode->Name(), XML_TAG_Problem))
		return new CProblem(pNode);

	return nullptr;
}

CProblem * CProblem::loadFromFile(std::string fileName)
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile(fileName.c_str());

	if (doc.Error() == tinyxml2::XML_SUCCESS)
	{
		tinyxml2::XMLElement *pRoot = doc.RootElement();
		if (pRoot)
		{
			return CProblem::getInstance(pRoot);
		}
	}

	return nullptr;
}

CProblem * CProblem::loadFromString(std::string content)
{
	tinyxml2::XMLDocument doc;

	if (doc.Parse(content.c_str()) == tinyxml2::XML_SUCCESS)
	{
		tinyxml2::XMLElement *pRoot = doc.RootElement();
		if (pRoot)
		{
			return CProblem::getInstance(pRoot);
		}
	}

	return nullptr;
}

CNetwork * CProblem::createNetwork()
{
	CNetwork* result = new CNetwork();
	/*
	basic idea of this algorithm: we traverse all chains to create the real model
	to achieve this we follow this pseudo-code:

	while output_node has not been reached:
	for each chain  in chains
	traverse chain as far as possible

	stop the processing when no changes are possible anymore -> the structure is faulty!

	*/

	//create the inputs
	for each (CInputData* input in m_inputs)
	{
		if (input->getInputFunctionPtr() == nullptr)
		{
			input->createInputFunctionPtr();
			result->getInputs().push_back(input);
		}
	}

	//output node
	CNTK::FunctionPtr pOutputFunction = nullptr;

	//stores the progress of each chain
	auto progressMap = std::map<CChain*, CLink*>();

	//the currently processed link
	CLink* pCurrentLink;

	//the dependencies of the current link
	vector<const CLink*> dependencies;

	//termination condition
	bool architectureDeadEndReached = false;

	//traverse the architecture until either the output has been created
	//or no more changes are applied
	while (pOutputFunction == nullptr && !architectureDeadEndReached)
	{
		architectureDeadEndReached = true;

		for each (CChain* pChain in m_pNetworkArchitecture->getChains())
		{
			//get current link of the chain
			if (progressMap.find(pChain) == progressMap.end())
			{
				//start at the beginning
				pCurrentLink = pChain->getChainLinks().at(0);
			}
			else
			{
				//resume the progress
				pCurrentLink = progressMap.at(pChain);
			}

			//go down in this chain until it is not any more possible
			while (pCurrentLink != nullptr)
			{
				//get dependencies
				dependencies = pCurrentLink->getDependencies();

				//check if dependencies have already been traversed
				bool dependencies_satisfied = true;
				for each (const CLink* item in dependencies)
				{
					if (item->getFunctionPtr() == nullptr)
					{
						dependencies_satisfied = false;
						break;
					}
				}

				//dependencies not satisfied -> try to process another chain
				if (!dependencies_satisfied)
					break;

				//all dependencies are satisfied and already traversed
				//create the FunctionPtr now and save it!

				//create CNTK node
				pCurrentLink->createCNTKFunctionPtr(dependencies);
				result->getFunctionPtrs().push_back(pCurrentLink->getFunctionPtr());

				//pCurrentLink->getFunctionPtr()->Save(L"C:\\Users\\Roland\\Desktop\\graph.dat");

				//check if output has been reached already
				if (!strcmp(m_pOutput->getTargetId().c_str(), pCurrentLink->getId().c_str()))
				{
					pOutputFunction = pCurrentLink->getFunctionPtr()->Output();
					result->getOutputsFunctionPtr().push_back(pOutputFunction);
					break;
				}

				//update current link
				pCurrentLink = pCurrentLink->getNextLink();

				//keep the loop alive
				architectureDeadEndReached = false;

			}

			//the output node has already been created
			//the network is created
			//all other links are not used to calculate the output
			if (pOutputFunction != nullptr)
				break;

			//store last processed item of the current chain
			progressMap.insert_or_assign(pChain, pCurrentLink);
		}
	}

	return result;
}