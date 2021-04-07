/*
	SimionZoo: A framework for online model-free Reinforcement Learning on continuous
	control problems

	Copyright (c) 2016 SimionSoft. https://github.com/simionsoft

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

#include "stdafx.h"
#include "collada-model.h"
#include "material.h"
#include "xml-load.h"
#include "renderer.h"
#include "texture-manager.h"
#include "mesh.h"
#include "../GeometryLib/bounding-cylinder.h"
#include <algorithm>
#include <stdexcept>
#include "../System/FileUtils.h"

void addValues(vector<unsigned int> source, unsigned int sourceOffset, unsigned int numValues, vector<unsigned int>& dest)
{
	for (int i = 0; i < numValues; i++)
		dest.push_back(source[sourceOffset + i]);
}

void parseIntArray(const char* pInputString, vector<unsigned int>& values)
{
	unsigned int numParsedValues = 0;
	const char* pt = pInputString;
	if (*pt == ' ' || *pt == '\n') ++pt; //skip initial space/line jump
	while (*pt != 0)
	{
		values.push_back(atoi(pt));

		while (*pt != 0 && *pt != ' ' && *pt != '\n') ++pt;
		if (*pt == ' ' || *pt == '\n') ++pt;
	}
}

unsigned int parseIntArray(const char* pInputString, unsigned int* pValues, unsigned int numValues, unsigned int startOffset= 0)
{
	unsigned int numParsedValues = 0;
	const char* pt = pInputString;
	if (*pt == ' ' || *pt == '\n') ++pt; //skip initial space/line jump
	while (*pt != 0 && (startOffset+numParsedValues)<numValues)
	{
		pValues[startOffset + numParsedValues++] = (atoi(pt));

		while (*pt != 0 && *pt != ' ' && *pt != '\n') ++pt;
		if (*pt == ' ' || *pt == '\n') ++pt;
	}
	return numParsedValues;
}

unsigned int parseFloatArray(const char* pInputString, double* pBuffer, unsigned int bufferSize)
{
	unsigned int numParsedValues = 0;
	const char* pt = pInputString;
	if (*pt == ' ' || *pt == '\n') ++pt; //skip initial space/line jump
	while (*pt != 0 && numParsedValues<bufferSize)
	{
		pBuffer[numParsedValues++] = (atof(pt));

		while (*pt != 0 && *pt != ' ' && *pt != '\n') ++pt;
		if (*pt == ' ' || *pt == '\n') ++pt;
	}
	return numParsedValues;
}

unsigned int parseFloatArrayToVector3DArray(const char* pInputString,Vector3D* pBuffer, unsigned int bufferElementSize)
{
	unsigned int numParsedValues = 0;
	unsigned int numParsedElements = 0;
	const char* pt = pInputString;
	if (*pt == ' ' || *pt == '\n') ++pt; //skip initial space/line jump
	while (*pt != 0 && numParsedElements<bufferElementSize)
	{
		switch (numParsedValues % 3)
		{
		case 0:pBuffer[numParsedElements].setX((atof(pt))); break;
		case 1:pBuffer[numParsedElements].setY((atof(pt))); break;
		case 2:pBuffer[numParsedElements].setZ((atof(pt))); ++numParsedElements; break;
		}
		++numParsedValues;

		while (*pt != 0 && *pt != ' ' && *pt != '\n') ++pt;
		if (*pt == ' ' || *pt == '\n') ++pt;
	}
	return numParsedElements;
}
unsigned int parseFloatArrayToVector2DArray(const char* pInputString, Vector2D* pBuffer, unsigned int bufferElementSize)
{
	unsigned int numParsedValues = 0;
	unsigned int numParsedElements = 0;
	const char* pt = pInputString;
	if (*pt == ' ' || *pt == '\n') ++pt; //skip initial space/line jump
	while (*pt != 0 && numParsedElements<bufferElementSize)
	{
		switch (numParsedValues % 2)
		{
		case 0:pBuffer[numParsedElements].setX((atof(pt))); break;
		case 1:pBuffer[numParsedElements].setY((atof(pt))); ++numParsedElements; break;
		}
		++numParsedValues;

		while (*pt != 0 && *pt != ' ' && *pt != '\n') ++pt;
		if (*pt == ' ' || *pt == '\n') ++pt;
	}
	return numParsedElements;
}

tinyxml2::XMLElement* deepSearch(tinyxml2::XMLElement* pNode, const char* elementName
	, const char* attributeName, const char* attributeValueToMatch)
{
	tinyxml2::XMLElement* pChild = pNode->FirstChildElement();
	tinyxml2::XMLElement* pReturned;
	while (pChild!=nullptr)
	{
		if (!strcmp(pChild->Name(), elementName) && pChild->Attribute(attributeName) != nullptr
			&& !strcmp(pChild->Attribute(attributeName), attributeValueToMatch))
			return pChild;
		pReturned = deepSearch(pChild, elementName, attributeName, attributeValueToMatch);
		if (pReturned != nullptr)
			return pReturned;
		pChild = pChild->NextSiblingElement();
	}
	return nullptr;
}


Mesh* ColladaModel::loadMesh(tinyxml2::XMLElement* pRoot, tinyxml2::XMLElement* pNode, Geometry* pGeometry, Matrix44& nodeTransform)
{
	Mesh* pMesh;
	Source* pPosSource = 0;
	Source* pNormSource = 0;
	Source* pTexCoordSource = 0;
	MultiInputDef m_definitions;
	int m_numIndicesPerPrimitive = 0;
	bool bVertexDefCounted = false;

	if (!pNode) return nullptr;

	pMesh = new Mesh();
	//material
	string materialName;
	if (pNode->Attribute(XML_TAG_COLLADA_MATERIAL_ATTR) != nullptr)
	{
		materialName = pNode->Attribute(XML_TAG_COLLADA_MATERIAL_ATTR);
		pMesh->setMaterial( loadMaterial(pRoot, materialName) );
	}
	//load input definitions
	m_definitions.load(pNode);

	//find the source with the vertex positions
	if (m_definitions.defines(XML_TAG_COLLADA_POSITION_SEMANTIC))
	{
		pPosSource = pGeometry->getSource(m_definitions.getSourceName(XML_TAG_COLLADA_POSITION_SEMANTIC)
			, XML_TAG_COLLADA_POSITION_SEMANTIC);
		++m_numIndicesPerPrimitive;
	}
	else
	{
		if (m_definitions.defines(XML_TAG_COLLADA_VERTEX_SEMANTIC))
		{
			if (!bVertexDefCounted) { ++m_numIndicesPerPrimitive; bVertexDefCounted = true; }
			pPosSource = pGeometry->getSource(m_definitions.getSourceName(XML_TAG_COLLADA_VERTEX_SEMANTIC)
				, XML_TAG_COLLADA_POSITION_SEMANTIC);
		}
	}
	//copy
	if (pPosSource)
	{
		pMesh->allocPositions(pPosSource->getNumElements());
		parseFloatArrayToVector3DArray(pPosSource->getAsString(), pMesh->getPosArray(), pPosSource->getNumElements());
	}

	//find the source with the vertex normals
	if (m_definitions.defines(XML_TAG_COLLADA_NORMAL_SEMANTIC))
	{
		pNormSource = pGeometry->getSource(m_definitions.getSourceName(XML_TAG_COLLADA_NORMAL_SEMANTIC)
			, XML_TAG_COLLADA_NORMAL_SEMANTIC);
		++m_numIndicesPerPrimitive;
	}
	else
	{
		if (m_definitions.defines(XML_TAG_COLLADA_VERTEX_SEMANTIC))
		{
			if (!bVertexDefCounted) { ++m_numIndicesPerPrimitive; bVertexDefCounted = true; }
			pNormSource = pGeometry->getSource(m_definitions.getSourceName(XML_TAG_COLLADA_VERTEX_SEMANTIC)
				, XML_TAG_COLLADA_NORMAL_SEMANTIC);
		}
	}
	//copy
	if (pNormSource)
	{
		pMesh->allocNormals(pNormSource->getNumElements());
		parseFloatArrayToVector3DArray(pNormSource->getAsString(), pMesh->getNormalArray(), pNormSource->getNumElements());
	}
	//find the source
	if (m_definitions.defines(XML_TAG_COLLADA_TEXCOORD_SEMANTIC))
	{
		pTexCoordSource = pGeometry->getSource(m_definitions.getSourceName(XML_TAG_COLLADA_TEXCOORD_SEMANTIC)
			, XML_TAG_COLLADA_TEXCOORD_SEMANTIC);
		++m_numIndicesPerPrimitive;
	}
	else
	{
		if (m_definitions.defines(XML_TAG_COLLADA_VERTEX_SEMANTIC))
		{
			if (!bVertexDefCounted) { ++m_numIndicesPerPrimitive; bVertexDefCounted = true; }
			pTexCoordSource = pGeometry->getSource(m_definitions.getSourceName(XML_TAG_COLLADA_VERTEX_SEMANTIC)
				, XML_TAG_COLLADA_TEXCOORD_SEMANTIC);
		}
	}
	//copy
	if (pTexCoordSource)
	{
		pMesh->allocTexCoords(pTexCoordSource->getNumElements());
		parseFloatArrayToVector2DArray(pTexCoordSource->getAsString(), pMesh->getTexCoordArray(), pTexCoordSource->getNumElements());
	}
	

	//we are not instancing geometries, but replicating them, so we can take a shortcut here and transform vertices
	//using the node's transform in the visual scene
	for (unsigned int i = 0; i < pMesh->getNumPositions(); ++i)
		pMesh->getPosition(i) = nodeTransform*pMesh->getPosition(i);
	for (unsigned int i = 0; i < pMesh->getNumNormals(); ++i)
	{
		pMesh->getNormal(i) = nodeTransform*pMesh->getNormal(i);
		pMesh->getNormal(i).normalize();
	}

	//up axis??
	if (pRoot->FirstChildElement(XML_TAG_COLLADA_ASSET) != nullptr
		&& pRoot->FirstChildElement(XML_TAG_COLLADA_ASSET)->FirstChildElement(XML_TAG_COLLADA_UP_AXIS) != nullptr
		&& !strcmp(pRoot->FirstChildElement(XML_TAG_COLLADA_ASSET)->FirstChildElement(XML_TAG_COLLADA_UP_AXIS)->GetText()
			, XML_TAG_COLLADA_Z_UP))
	{
		pMesh->flipYZAxis();
		pMesh->flipVTexCoord();
	}

	//indices

	m_numIndicesPerPrimitive = std::max(1, m_numIndicesPerPrimitive);
	int numIndices = atoi(pNode->Attribute(XML_TAG_COLLADA_COUNT_ATTR));

	//check if there is a <vcount> element with the number of vertices per polygon
	tinyxml2::XMLElement* pVertexCounts = pNode->FirstChildElement(XML_TAG_COLLADA_VERTEX_COUNT);
	vector<unsigned int> vertexCounts;
	if (pVertexCounts != nullptr)
	{
		char* pCharArray = (char*)pVertexCounts->GetText();
		parseIntArray(pCharArray, pMesh->getVertexCountArray());
	}


	//do different attributes share vertices?
	if (m_numIndicesPerPrimitive >1)
	{
		pMesh->setPosOffset(0);
		pMesh->setNormalOffset(1);
		pMesh->setTexCoordOffset(2);
		pMesh->setNumIndicesPerVertex(m_numIndicesPerPrimitive);
	}

	//parse indices
	int numParsedPrimitives = 0;
	tinyxml2::XMLElement* pPrimitive = pNode->FirstChildElement(XML_TAG_COLLADA_PRIMITIVE);
	while (pPrimitive != nullptr)
	{
		char* pCharArray = (char*)pPrimitive->GetText();
		vector<unsigned int> parsedIndices;
		parseIntArray(pCharArray, parsedIndices);

		if (pMesh->getVertexCountArray().size() > 0)
		{
			//pMesh->ignoreTexCoordsAndNormals(); //so far, all models with <vcount> have only an index per vertex even if they should have more
			
			int primitiveIndicesOffset = 0;
			for (int i = 0; i < pMesh->getVertexCountArray().size(); i++)
			{
				if (pMesh->getVertexCountArray()[i] == 3)
				{
					//triangle
					addValues(parsedIndices, primitiveIndicesOffset, 1, pMesh->getIndexArray());
					addValues(parsedIndices, primitiveIndicesOffset + 1, 1, pMesh->getIndexArray());
					addValues(parsedIndices, primitiveIndicesOffset + 2, 1, pMesh->getIndexArray());
				}
				else if (pMesh->getVertexCountArray()[i] == 4)
				{
					//quad
					addValues(parsedIndices, primitiveIndicesOffset, 1, pMesh->getIndexArray());
					addValues(parsedIndices, primitiveIndicesOffset + 1, 1, pMesh->getIndexArray());
					addValues(parsedIndices, primitiveIndicesOffset + 2, 1, pMesh->getIndexArray());

					addValues(parsedIndices, primitiveIndicesOffset, 1, pMesh->getIndexArray());
					addValues(parsedIndices, primitiveIndicesOffset + 2, 1, pMesh->getIndexArray());
					addValues(parsedIndices, primitiveIndicesOffset + 3, 1, pMesh->getIndexArray());
				}
				primitiveIndicesOffset += pMesh->getVertexCountArray()[i];
			}
		}
		else
		{
			for (int i = 0; i < parsedIndices.size(); i++)
				pMesh->getIndexArray().push_back(parsedIndices[i]);
		}

		pPrimitive = pPrimitive->NextSiblingElement(XML_TAG_COLLADA_PRIMITIVE);
	}
	
	return pMesh;
}


MultiInputDef::MultiInputDef() {}

MultiInputDef::~MultiInputDef() {}

void MultiInputDef::load(tinyxml2::XMLElement* pNode)
{
	InputDef inputDef;
	tinyxml2::XMLElement* pInput = pNode->FirstChildElement(XML_TAG_COLLADA_INPUT);
	while (pInput != nullptr)
	{
		inputDef.semantic = pInput->Attribute(XML_TAG_COLLADA_SEMANTIC_ATTR);
		if (!defines(inputDef.semantic))
		{
			//re-definitions are ignored to avoid second sets of texture coordinates, which are not yet supported
			if (pInput->Attribute(XML_TAG_COLLADA_OFFSET_ATTR) != nullptr)
				inputDef.offset = atoi(pInput->Attribute(XML_TAG_COLLADA_OFFSET_ATTR));
			else inputDef.offset = 0;
			inputDef.source = string(pInput->Attribute(XML_TAG_COLLADA_SOURCE)).substr(1); //new string without the '#' character
			m_definitions.push_back(inputDef);
		}

		pInput = pInput->NextSiblingElement(XML_TAG_COLLADA_INPUT);
	}
}

string MultiInputDef::getSourceName(string semantic)
{
	for (auto it = m_definitions.begin(); it != m_definitions.end(); ++it)
	{
		if ((*it).semantic == semantic)
			return (*it).source;
	}
	throw std::runtime_error((string("Input definition not found: ") + semantic).c_str());
}

bool MultiInputDef::defines(string semantic)
{
	for (auto it = m_definitions.begin(); it != m_definitions.end(); ++it)
	{
		if ((*it).semantic == semantic)
			return true;
	}
	return false;
}

VertexDef::VertexDef()
{}

VertexDef::~VertexDef()
{}

void VertexDef::load(tinyxml2::XMLElement* pNode)
{
	if (pNode->Attribute(XML_TAG_COLLADA_ID_ATTR) != nullptr)
		m_definitionId = pNode->Attribute(XML_TAG_COLLADA_ID_ATTR);

	m_inputDefinition.load(pNode);
}

string VertexDef::getSourceName(string semantic)
{
	if (m_inputDefinition.defines(semantic))
		return m_inputDefinition.getSourceName(semantic);
	return string("");
}

Source::Source(Geometry* pParent): m_pParent(pParent)
{}

Source::~Source()
{
}

void Source::loadAccesor(tinyxml2::XMLElement* pNode)
{
	const char* pName;
	int offset = 0;
	tinyxml2::XMLElement* pParam = pNode->FirstChildElement(XML_TAG_COLLADA_PARAM);
	while (pParam != nullptr)
	{
		pName= pParam->Attribute(XML_TAG_COLLADA_NAME);
		if (pName != nullptr)
		{
			m_sourceAccesors[string(pName)] = offset;
			++offset;
		}
		pParam = pParam->NextSiblingElement(XML_TAG_COLLADA_PARAM);
	}
	m_elementStride = offset;
}

void Source::loadArray(tinyxml2::XMLElement* pNode)
{
	if (pNode->Attribute(XML_TAG_COLLADA_ID_ATTR) != nullptr)
		m_arrayId = string(pNode->Attribute(XML_TAG_COLLADA_ID_ATTR));
	if (pNode->Attribute(XML_TAG_COLLADA_COUNT_ATTR) != nullptr)
		m_valueCount = atoi(pNode->Attribute(XML_TAG_COLLADA_COUNT_ATTR));
	else return;

	//save the pointer to the buffer
	m_pFloatArrayAsString= (char*)pNode->GetText();
}

void Source::load(tinyxml2::XMLElement* pNode)
{
	tinyxml2::XMLElement *pArray, *pTechnique, *pAccesor;
	if (pNode->Attribute(XML_TAG_COLLADA_ID_ATTR) != nullptr)
		m_id = string(pNode->Attribute(XML_TAG_COLLADA_ID_ATTR));

	pArray = pNode->FirstChildElement(XML_TAG_COLLADA_FLOAT_ARRAY);
	if ( pArray != nullptr)
		loadArray(pArray);

	pTechnique = pNode->FirstChildElement(XML_TAG_COLLADA_TECHNIQUE_COMMON);
	if (pTechnique!=nullptr)
	{
		pAccesor = pTechnique->FirstChildElement(XML_TAG_COLLADA_ACCESOR);
		if (pAccesor != nullptr)
		{
			loadAccesor(pAccesor);
			if (m_elementStride!= 0)
				m_numElements = m_valueCount / m_elementStride;
		}
	}
}

Geometry::Geometry(ColladaModel* pParent): m_pParent(pParent)
{}

Geometry::~Geometry()
{
	for (auto it = m_sources.begin(); it != m_sources.end(); ++it)
		delete *it;
}

void Geometry::load(tinyxml2::XMLElement* pRoot, tinyxml2::XMLElement* pNode, Matrix44& nodeTransform)
{
	Source* pSource;
	tinyxml2::XMLElement* pMeshNode = pNode->FirstChildElement(XML_TAG_COLLADA_MESH);

	//load sources
	tinyxml2::XMLElement* pSourceCfg = pMeshNode->FirstChildElement(XML_TAG_COLLADA_SOURCE);
	while (pSourceCfg != nullptr)
	{
		pSource = new Source(this);
		pSource->load(pSourceCfg);
		m_sources.push_back(pSource);

		pSourceCfg = pSourceCfg->NextSiblingElement(XML_TAG_COLLADA_SOURCE);
	}

	//load vertex definitions
	tinyxml2::XMLElement* pVertexDef = pMeshNode->FirstChildElement(XML_TAG_COLLADA_VERTICES);
	if (pVertexDef != nullptr)
		m_vertexDef.load(pVertexDef);

	//load meshes
	tinyxml2::XMLElement* pPrimitiveNode;
	Mesh* pMesh;
	vector<string> primitives = { XML_TAG_COLLADA_TRIANGLES,XML_TAG_COLLADA_POLYLIST,XML_TAG_COLLADA_POLYGONS };
	for (auto it = primitives.begin(); it != primitives.end(); ++it)
	{
		pPrimitiveNode = pMeshNode->FirstChildElement((*it).c_str());
		while (pPrimitiveNode != nullptr)
		{
			pMesh = m_pParent->loadMesh(pRoot,pPrimitiveNode, this, nodeTransform);
			if (pMesh!=nullptr)
				m_pParent->addMesh(pMesh);

			pPrimitiveNode = pPrimitiveNode->FirstChildElement((*it).c_str());
		}
	}
	
}

Source* Geometry::getSource(string inputName, string requestedSemantic)
{
	if (inputName == m_vertexDef.id())
		inputName = m_vertexDef.getSourceName(requestedSemantic);

	for (auto it = m_sources.begin(); it != m_sources.end(); ++it)
	{
		if ((*it)->getId() == inputName)
			return *it;
	}
	return nullptr;
}

ColladaModel::ColladaModel(tinyxml2::XMLElement* pNode): GraphicObject3D(pNode)
{
	tinyxml2::XMLElement* pChild;

	m_path = string(pNode->Attribute(XML_TAG_PATH_ATTR));

	string pDir= getDirectory(m_path.c_str());
	string dataFolder = Renderer::get()->getDataFolder();
	string previousTextureFolder= Renderer::get()->getTextureManager()->getFolder();
	Renderer::get()->getTextureManager()->setFolder(dataFolder + pDir);

	loadFromFile(m_path.c_str());

	Renderer::get()->getTextureManager()->setFolder(previousTextureFolder);

	updateBoundingBox();

	//do we need to rescale the model's coordinates to fit a bounding box?
	BoundingBox3D fitBB;
	pChild = pNode->FirstChildElement(XML_TAG_FIT_BOUNDING_BOX);
	if (pChild)
	{
		XML::load(pChild, fitBB);
		fitBB.set(true); //mark bounding box as set
		fitToBoundingBox(&fitBB);
	}
	BoundingCylinder fitBC;
	pChild = pNode->FirstChildElement(XML_TAG_FIT_BOUNDING_CYLINDER);
	if (pChild)
	{
		XML::load(pChild,fitBC);
		fitToBoundingCylinder(&fitBC);
	}
}

void ColladaModel::instanceGeometry(tinyxml2::XMLElement* pRootNode, const char* geometryId, Matrix44& nodeTransform)
{
	//load the meshes
	Geometry geometry(this);
	tinyxml2::XMLElement *pGeometryNode = pRootNode->FirstChildElement(XML_TAG_COLLADA_LIB_GEOM)
		->FirstChildElement(XML_TAG_COLLADA_GEOMETRY);
	while (pGeometryNode != nullptr)
	{
		if (!strcmp(pGeometryNode->Attribute(XML_TAG_COLLADA_ID_ATTR), geometryId))
		{
			geometry.load(pRootNode, pGeometryNode, nodeTransform);
			return;
		}

		pGeometryNode = pGeometryNode->NextSiblingElement(XML_TAG_COLLADA_GEOMETRY);
	}
	return;
}

void ColladaModel::traverseVisualScene(tinyxml2::XMLElement* pRootNode, tinyxml2::XMLElement* pNode, Matrix44& nodeTransform)
{
	//node has a transform matrix??
	Vector3D translation;
	Matrix44 localTransform, readTransform,translationMatrix;
	readTransform.setIdentity();
	if (pNode->FirstChildElement(XML_TAG_COLLADA_MATRIX) != nullptr)
	{
		XML::loadColladaMatrix(pNode->FirstChildElement(XML_TAG_COLLADA_MATRIX)->GetText(), readTransform);
		localTransform = readTransform*nodeTransform;
	}
	else if (pNode->FirstChildElement(XML_TAG_COLLADA_TRANSLATE))
	{
		XML::loadVector3D(pNode->FirstChildElement(XML_TAG_COLLADA_TRANSLATE)->GetText(), translation);
		translationMatrix.setTranslation(translation);
		localTransform = translationMatrix*nodeTransform;
	}
	else localTransform = nodeTransform;

	//instance geometry??
	const char* geometryId;
	tinyxml2::XMLElement* pGeometryInstance = pNode->FirstChildElement(XML_TAG_COLLADA_GEOMETRY_INSTANCE);
	if (pGeometryInstance)
	{
		geometryId = pGeometryInstance->Attribute(XML_TAG_COLLADA_URL) + 1;
		instanceGeometry(pRootNode, geometryId, localTransform);
	}

	//traverse child nodes
	tinyxml2::XMLElement *pChildNode = pNode->FirstChildElement(XML_TAG_COLLADA_NODE);
	while (pChildNode != nullptr)
	{
		traverseVisualScene(pRootNode, pChildNode, nodeTransform);
		pChildNode = pChildNode->NextSiblingElement(XML_TAG_COLLADA_NODE);
	}
}

void ColladaModel::loadVisualScenes(tinyxml2::XMLElement* pRootNode)
{
	//load the visual scene
	tinyxml2::XMLElement* pVisualScene, *pNode;
	tinyxml2::XMLElement* pVisualSceneLib = pRootNode->FirstChildElement(XML_TAG_COLLADA_VISUAL_SCENE_LIB);
	Matrix44 nodeTransform;
	nodeTransform.setIdentity();
	if (pVisualSceneLib)
	{
		pVisualScene = pVisualSceneLib->FirstChildElement(XML_TAG_COLLADA_VISUAL_SCENE);
		if (pVisualScene)
		{
			pNode = pVisualScene->FirstChildElement(XML_TAG_COLLADA_NODE);
			while(pNode!=nullptr)
			{
				traverseVisualScene(pRootNode, pNode, nodeTransform);
				pNode = pNode->NextSiblingElement(XML_TAG_COLLADA_NODE);
			}
		}
	}
}

void ColladaModel::loadSkin(tinyxml2::XMLElement* pRootNode)
{
	Matrix44 matrix;
	matrix.setIdentity();

	const char* geometryId;
	tinyxml2::XMLElement* pController, *pSkin;
	tinyxml2::XMLElement* pControllerLib = pRootNode->FirstChildElement(XML_TAG_COLLADA_CONTROLLER_LIB);
	if (pControllerLib)
	{
		pController = pControllerLib->FirstChildElement(XML_TAG_COLLADA_CONTROLLER);
		while (pController)
		{
			pSkin = pController->FirstChildElement(XML_TAG_COLLADA_SKIN);
			if (pSkin && pSkin->Attribute(XML_TAG_COLLADA_SOURCE))
			{
				if (pSkin->FirstChildElement(XML_TAG_COLLADA_BIND_SHAPE_MATRIX))
					XML::loadColladaMatrix(pSkin->FirstChildElement(XML_TAG_COLLADA_BIND_SHAPE_MATRIX)->GetText(), matrix);
				geometryId = pSkin->Attribute(XML_TAG_COLLADA_SOURCE) + 1;
				instanceGeometry(pRootNode, geometryId, matrix);
			}

			pController = pController->NextSiblingElement(XML_TAG_COLLADA_CONTROLLER);
		}
	}
}


void ColladaModel::loadFromFile(const char* file)
{
	tinyxml2::XMLDocument doc;

	string path = Renderer::get()->getDataFolder() + string(file);

	Renderer::get()->logMessage("Opening Collada file" + path);
	doc.LoadFile(path.c_str());
	if (doc.Error() == tinyxml2::XML_SUCCESS)
	{
		Renderer::get()->logMessage("File found. Parsing");
		tinyxml2::XMLElement *pColladaRoot = doc.FirstChildElement(XML_TAG_COLLADA_ROOT);

		loadVisualScenes(pColladaRoot);

		//some collada files link the geometry only via the controller's skin
		//this is yet one more shortcut to get the geometry loaded
		loadSkin(pColladaRoot);
	}
	else Renderer::get()->logMessage("ERROR: Cound not open Collada file");
}

const char* ColladaModel::findTexture(tinyxml2::XMLElement* pRootNode, string textureName)
{
	static string imageFile;
	tinyxml2::XMLElement* pImageLib = pRootNode->FirstChildElement(XML_TAG_COLLADA_IMAGE_LIB);
	if (pImageLib != nullptr)
	{
		tinyxml2::XMLElement* pImage = pImageLib->FirstChildElement(XML_TAG_COLLADA_IMAGE);
		while (pImage != nullptr)
		{
			if (pImage->Attribute(XML_TAG_COLLADA_ID_ATTR) == textureName)
			{
				imageFile = string(pImage->FirstChildElement(XML_TAG_COLLADA_INIT)->GetText())
					.substr(string(XML_TAG_COLLADA_IMAGE_FILE_PREFIX).size());

				size_t offset = imageFile.find_last_of('/');
				if (offset > 0)
					imageFile = imageFile.substr(offset + 1);
				return imageFile.c_str();
			}
			pImage = pImage->NextSiblingElement(XML_TAG_COLLADA_IMAGE);
		}
	}
	return nullptr;
}

const char* ColladaModel::findTextureRedirection(tinyxml2::XMLElement* pFxProfile, string textureName)
{
	tinyxml2::XMLElement* pRedir;
	tinyxml2::XMLElement* pNewParam = pFxProfile->FirstChildElement(XML_TAG_COLLADA_NEWPARAM);
	while (pNewParam != nullptr)
	{
		if (string(pNewParam->Attribute(XML_TAG_COLLADA_SID)) == textureName)
		{
			//<newparam sid = "Image-sampler">
			//	<sampler2D>
			//	<source>Image - surface< / source>
			pRedir = pNewParam->FirstChildElement()->FirstChildElement(XML_TAG_COLLADA_SOURCE);
			if (pRedir != nullptr)
				return pRedir->GetText();
			//<newparam sid = "Image-surface">
			//	<surface type = "2D">
			//	<init_from>Image< / init_from>
			pRedir = pNewParam->FirstChildElement()->FirstChildElement(XML_TAG_COLLADA_INIT);
			if (pRedir != nullptr)
				return pRedir->GetText();
			//some unknown type of redirection
			return nullptr;
		}
		pNewParam = pNewParam->NextSiblingElement(XML_TAG_COLLADA_NEWPARAM);
	}
	return nullptr;
}

int ColladaModel::loadTexture(tinyxml2::XMLElement* pRootNode, tinyxml2::XMLElement* pFxProfile, string textureName)
{
	size_t textureId;
	//first, we assume the number of the texture in the diffuse element is the name of a texture resource
	const char* imageFile = findTexture(pRootNode, textureName);
	if (imageFile == nullptr)
	{
		//we look for a newParam element within the fx profile that redirects the texture
		//max. 2 levels of indirection. Haven't seen any deeper
		textureName = findTextureRedirection(pFxProfile,textureName);
		imageFile = findTexture(pRootNode, textureName);
		if (imageFile == nullptr)
		{
			textureName = findTextureRedirection(pFxProfile, textureName);
			imageFile = findTexture(pRootNode, textureName);
		}
	}
	if (imageFile != nullptr)
	{
		textureId = Renderer::get()->getTextureManager()->loadTexture(imageFile);
		return (int)textureId;
	}

	return -1;
}

void ColladaModel::loadMaterialProperties(tinyxml2::XMLElement* pRootNode
	, const char* fxName, SimpleTLMaterial* pMaterial)
{
	string textureName;
	double rgba[4];
	tinyxml2::XMLElement* pFxLib = pRootNode->FirstChildElement(XML_TAG_COLLADA_FX_LIB);
	if (!pFxLib) return;
	tinyxml2::XMLElement* pFx = pFxLib->FirstChildElement(XML_TAG_COLLADA_FX);
	while (pFx != nullptr)
	{
		if (!strcmp(pFx->Attribute(XML_TAG_COLLADA_ID_ATTR),fxName))
		{
			tinyxml2::XMLElement* pProfile = pFx->FirstChildElement(XML_TAG_COLLADA_PROFILE_FX);
			tinyxml2::XMLElement* pMatProp = pProfile->FirstChildElement(XML_TAG_COLLADA_TECHNIQUE)->FirstChildElement();
			//AMBIENT
			if (pMatProp->FirstChildElement(XML_TAG_COLLADA_AMBIENT) != nullptr)
			{
				parseFloatArray(pMatProp->FirstChildElement(XML_TAG_COLLADA_AMBIENT)
					->FirstChildElement(XML_TAG_COLLADA_COLOR)->GetText(), rgba, 4);
				pMaterial->setAmbient(Color((float)rgba[0], (float)rgba[1], (float)rgba[2], (float)rgba[3]));
			}
			//SPECULAR
			if (pMatProp->FirstChildElement(XML_TAG_COLLADA_SPECULAR)!=nullptr)
			{
				parseFloatArray(pMatProp->FirstChildElement(XML_TAG_COLLADA_SPECULAR)
					->FirstChildElement(XML_TAG_COLLADA_COLOR)->GetText(), rgba, 4);
				pMaterial->setSpecular(Color((float)rgba[0], (float)rgba[1], (float)rgba[2], (float)rgba[3]));
			}
			//EMISSION
			if (pMatProp->FirstChildElement(XML_TAG_COLLADA_EMISSION) != nullptr)
			{
				parseFloatArray(pMatProp->FirstChildElement(XML_TAG_COLLADA_EMISSION)
					->FirstChildElement(XML_TAG_COLLADA_COLOR)->GetText(), rgba, 4);
				pMaterial->setEmission(Color((float)rgba[0], (float)rgba[1], (float)rgba[2], (float)rgba[3]));
			}
			//SHININESS
			if (pMatProp->FirstChildElement(XML_TAG_COLLADA_SHININESS) != nullptr)
			{
				pMaterial->setShininess(atof(pMatProp->FirstChildElement(XML_TAG_COLLADA_SHININESS)
					->FirstChildElement(XML_TAG_COLLADA_FLOAT)->GetText()));
			}
			//DIFFUSE
			if (pMatProp->FirstChildElement(XML_TAG_COLLADA_DIFFUSE) != nullptr)
			{
				if (pMatProp->FirstChildElement(XML_TAG_COLLADA_DIFFUSE)
					->FirstChildElement(XML_TAG_COLLADA_COLOR) != nullptr)
				{
					parseFloatArray(pMatProp->FirstChildElement(XML_TAG_COLLADA_DIFFUSE)
						->FirstChildElement(XML_TAG_COLLADA_COLOR)->GetText(), rgba, 4);
					pMaterial->setDiffuse(Color((float)rgba[0], (float)rgba[1], (float)rgba[2], (float)rgba[3]));
				}

				//DIFFUSE-TEXTURE
				tinyxml2::XMLElement* pTexture = pMatProp->FirstChildElement(XML_TAG_COLLADA_DIFFUSE)
					->FirstChildElement(XML_TAG_COLLADA_TEXTURE);
				if (pTexture != nullptr && pTexture->Attribute(XML_TAG_COLLADA_TEXTURE_ATTR) != nullptr)
				{
					textureName = pTexture->Attribute(XML_TAG_COLLADA_TEXTURE_ATTR);

					pMaterial->setTexture(loadTexture(pRootNode, pProfile, textureName));
				}
			}
		}
		pFx = pFx->NextSiblingElement(XML_TAG_COLLADA_FX);
	}
}

const char* ColladaModel::findMaterialFxName(tinyxml2::XMLElement* pRootNode, string materialName)
{
	tinyxml2::XMLElement* pMat, *pInstFX;
	tinyxml2::XMLElement* pMatLib = pRootNode->FirstChildElement(XML_TAG_COLLADA_MATERIAL_LIB);
	if (pMatLib)
	{
		pMat = pMatLib->FirstChildElement(XML_TAG_COLLADA_MATERIAL);
		while (pMat != nullptr)
		{
			if (materialName == pMat->Attribute(XML_TAG_COLLADA_ID_ATTR))
			{
				pInstFX = pMat->FirstChildElement(XML_TAG_COLLADA_INSTANCE_FX);
				if (pInstFX != nullptr)
					return &(pInstFX->Attribute(XML_TAG_COLLADA_URL))[1];
			}
			pMat = pMat->NextSiblingElement(XML_TAG_COLLADA_MATERIAL);
		}
	}
	return nullptr;
}

SimpleTLMaterial* ColladaModel::loadMaterial(tinyxml2::XMLElement* pRootNode, string materialName)
{
	SimpleTLMaterial* pMaterial;
	const char* fxName = nullptr;
	//find the material's effect
	string name;
	string textureName;

	fxName = findMaterialFxName(pRootNode, materialName);
	if (fxName ==nullptr)
	{
		//No direct definition of the material was found, must look for more complex material definitions
		//Search the "library_visual_scenes" for the material definition
		//<instance_material symbol="VisualMaterial0" target="#VisualMaterial"/>
		string target;
		tinyxml2::XMLElement* pSymbol= deepSearch(pRootNode->FirstChildElement(XML_TAG_COLLADA_VISUAL_SCENE_LIB)
			, XML_TAG_COLLADA_INSTANCE_MATERIAL, XML_TAG_COLLADA_SYMBOL_ATTR, materialName.c_str());
		if (pSymbol)
			materialName = &(pSymbol->Attribute(XML_TAG_COLLADA_TARGET))[1];
		//search for the target material
		fxName = findMaterialFxName(pRootNode, materialName);
	}

	if (fxName !=nullptr)
	{
		pMaterial = new SimpleTLMaterial();
		loadMaterialProperties(pRootNode,fxName,pMaterial);
		return pMaterial;
	}

	return nullptr;
}


ColladaModel::~ColladaModel()
{
}
