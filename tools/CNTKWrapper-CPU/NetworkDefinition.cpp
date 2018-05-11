#ifdef _WIN64

#include "NetworkDefinition.h"
#include "xmltags.h"
#include "InputData.h"
#include "NetworkArchitecture.h"
#include "Link.h"
#include "Chain.h"
#include "ParameterValues.h"
#include "Network.h"
#include "OptimizerSetting.h"
#include "Exceptions.h"
#include "Minibatch.h"
#include "CNTKWrapperInternals.h"
#include "../../RLSimion/named-var-set.h"

NetworkDefinition::NetworkDefinition(tinyxml2::XMLElement * pParentNode)
{
	//load architecture
	tinyxml2::XMLElement *pNode = pParentNode->FirstChildElement(XML_TAG_NETWORK_ARCHITECTURE);
	if (pNode == nullptr)
		throw ProblemParserElementNotFound(XML_TAG_NETWORK_ARCHITECTURE);

	m_pNetworkArchitecture = NetworkArchitecture::getInstance(pNode);
	m_pNetworkArchitecture->setParentProblem(this);

	//load output
	pNode = pParentNode->FirstChildElement(XML_TAG_Output);
	if (pNode == nullptr)
		throw ProblemParserElementNotFound(XML_TAG_Output);
	pNode = pNode->FirstChildElement(XML_TAG_LinkConnection);
	if (pNode == nullptr)
		throw ProblemParserElementNotFound(XML_TAG_LinkConnection);
	m_pOutput = LinkConnection::getInstance(pNode);

	//load optimizer
	pNode = pParentNode->FirstChildElement(XML_TAG_OptimizerSetting)->FirstChildElement(XML_TAG_Optimizer);
	if (pNode == nullptr)
		throw ProblemParserElementNotFound(XML_TAG_Optimizer);
	m_pOptimizerSetting = OptimizerSettings::getInstance(pNode);
}

NetworkDefinition::NetworkDefinition()
{
}

NetworkDefinition::~NetworkDefinition()
{
}

void NetworkDefinition::destroy()
{
	delete this;
}

NetworkDefinition * NetworkDefinition::getInstance(tinyxml2::XMLElement * pNode)
{
	if (!strcmp(pNode->Name(), XML_TAG_Problem))
		return new NetworkDefinition(pNode);

	return nullptr;
}

NetworkDefinition * NetworkDefinition::loadFromFile(std::string fileName)
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile(fileName.c_str());

	if (doc.Error() == tinyxml2::XML_SUCCESS)
	{
		tinyxml2::XMLElement *pRoot = doc.RootElement();
		if (pRoot)
		{
			return NetworkDefinition::getInstance(pRoot);
		}
	}

	return nullptr;
}

NetworkDefinition * NetworkDefinition::loadFromString(std::string content)
{
	tinyxml2::XMLDocument doc;

	if (doc.Parse(content.c_str()) == tinyxml2::XML_SUCCESS)
	{
		tinyxml2::XMLElement *pRoot = doc.RootElement();
		if (pRoot)
		{
			return NetworkDefinition::getInstance(pRoot);
		}
	}

	return nullptr;
}

INetwork * NetworkDefinition::createNetwork(double learningRate, bool inputsNeedGradient)
{
	m_bInputsNeedGradient = inputsNeedGradient;
	Network* result = new Network(this);

	/*
	basic idea of this algorithm: we traverse all chains to create the real model
	to achieve this we follow this pseudo-code:

	while output_node has not been reached:
	for each chain  in chains
	traverse chain as far as possible

	stop the processing when no changes are possible anymore -> the structure is faulty!

	*/

	//output node
	CNTK::FunctionPtr pOutputFunction = nullptr;

	//stores the progress of each chain
	auto progressMap = std::map<Chain*, Link*>();

	//the currently processed link
	Link* pCurrentLink;

	//the dependencies of the current link
	vector<const Link*> dependencies;

	//termination condition
	bool architectureDeadEndReached = false;

	//traverse the architecture until either the output has been created
	//or no more changes are applied
	while (pOutputFunction == nullptr && !architectureDeadEndReached)
	{
		architectureDeadEndReached = true;

		for each (Chain* pChain in m_pNetworkArchitecture->getChains())
		{
			//get current link of the chain
			if (progressMap.find(pChain) == progressMap.end())
			{
				//start at the beginning
				pCurrentLink = pChain->getLinks().at(0);
			}
			else
			{
				//resume the progress
				pCurrentLink = progressMap.at(pChain);
			}

			//go down in this chain until we can't go any further
			while (pCurrentLink != nullptr)
			{
				//get dependencies
				dependencies = pCurrentLink->getDependencies();

				//check if dependencies have already been traversed
				bool dependencies_satisfied = true;
				for each (const Link* item in dependencies)
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

				//check if output has been reached already
				if (m_pOutput->getTargetId() == pCurrentLink->getId())
				{
					pOutputFunction = pCurrentLink->getFunctionPtr()->Output();
					result->setOutputLayer(pOutputFunction);
					setOutputLayer(pOutputFunction->Name());
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

	result->buildNetwork(learningRate);
	return result;
}


void NetworkDefinition::addInputStateVar(string name, size_t id)
{
	m_inputStateVariables.push_back(name);
	m_inputStateVarIds.push_back(id);
}

const vector<size_t>& NetworkDefinition::getInputStateVarIds()
{
	return m_inputStateVarIds;
}

const vector<string>& NetworkDefinition::getInputStateVariables()
{
	return m_inputStateVariables;
}

void NetworkDefinition::addInputActionVar(string name, size_t id)
{
	m_inputActionVariables.push_back(name);
	m_inputActionVarIds.push_back(id);
}

const vector<size_t>& NetworkDefinition::getInputActionVarIds()
{
	return m_inputActionVarIds;
}

const vector<string>& NetworkDefinition::getInputActionVariables()
{
	return m_inputActionVariables;
}


size_t NetworkDefinition::getInputSize()
{
	return m_inputStateVarIds.size() + m_inputActionVarIds.size();
}


void NetworkDefinition::setDiscretizedActionVectorOutput(size_t actionVarId, size_t numOutputs, double minvalue, double maxvalue)
{
	double stepSize;

	m_outputType = DiscretizedActionVector;
	m_outputSize = numOutputs;

	m_outputActionValues = vector<double>(numOutputs);
	for (size_t i = 0; i < numOutputs; i++)
	{
		stepSize = (maxvalue - minvalue)/((int)numOutputs-1);
		m_outputActionValues[i] = minvalue + stepSize * i;
	}
}

double NetworkDefinition::getActionIndexOutput(size_t actionIndex)
{
	if (m_outputType != DiscretizedActionVector)
		throw std::exception("Can only use getActionIndexOutput() with discretized action vector outputs");

	actionIndex = std::max((size_t)0, std::min(m_outputActionValues.size() - 1, actionIndex));
	return m_outputActionValues[actionIndex];
}

size_t NetworkDefinition::getClosestOutputIndex(double value)
{
	if (m_outputType != DiscretizedActionVector)
		throw std::exception("Can only use getClosestOutputIndex() with discretized action vector outputs");

	size_t nearestIndex = 0;

	double dist;
	double closestDist = abs(value - m_outputActionValues[0]);

	for (size_t i = 1; i < m_outputActionValues.size(); i++)
	{
		dist = abs(value - m_outputActionValues[i]);
		//there is no special treatment for circular variables 
		if (dist < closestDist)
		{
			closestDist = dist;
			nearestIndex = i;
		}
	}

	return nearestIndex;
}

size_t NetworkDefinition::getOutputSize()
{
	return m_outputSize;
}

void NetworkDefinition::setScalarOutput()
{
	m_outputType = Scalar;
	m_outputSize = 1;
}

void NetworkDefinition::setVectorOutput(size_t dimension)
{
	m_outputType = Vector;
	m_outputSize = dimension;
}

IMinibatch* NetworkDefinition::createMinibatch(size_t sampleSize, size_t outputSize)
{
	return new Minibatch(sampleSize, this, outputSize);
}

void NetworkDefinition::setStateInputLayer(wstring name)
{
	m_stateInputLayer= name;
}

void NetworkDefinition::setActionInputLayer(wstring name)
{
	m_actionInputLayer = name;
}

wstring NetworkDefinition::getStateInputLayer()
{
	return m_stateInputLayer;
}

wstring NetworkDefinition::getActionInputLayer()
{
	return m_actionInputLayer;
}


void NetworkDefinition::setOutputLayer(wstring name)
{
	m_outputLayerName = name;
}
wstring NetworkDefinition::getOutputLayer()
{
	return m_outputLayerName;
}

#endif // _WIN64