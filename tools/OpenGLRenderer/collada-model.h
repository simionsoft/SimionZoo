#pragma once
#include "graphic-object.h"
#include "vector3d.h"
#include <vector>
#include <string>
#include <map>

namespace tinyxml2 { class XMLElement; }
class CColladaModel;
class CGeometry;
class CSource;
class CSimpleTLMaterial;
class CMesh;

using namespace std;

unsigned int parseFloatArrayToVector3DArray(const char* pInputString, Vector3D* pBuffer, unsigned int bufferElementSize);
unsigned int parseFloatArrayToVector2DArray(const char* pInputString, Vector2D* pBuffer, unsigned int bufferElementSize);
unsigned int parseIntArray(const char* pInputString, unsigned int* pBuffer, unsigned int bufferSize);
unsigned int parseFloatArray(const char* pInputString, double* pBuffer, unsigned int bufferSize);
tinyxml2::XMLElement* deepSearch(tinyxml2::XMLElement* pNode, const char* elementName
	, const char* attributeName, const char* attributeValueToMatch);

struct InputDef
{
	string semantic;
	int offset = 0;
	string source;
};

class CInputDef
{
	vector<InputDef> m_definitions;

public:
	CInputDef();
	~CInputDef();
	void load(tinyxml2::XMLElement* pNode);

	string getSourceName(string semantic);
	bool defines(string semantic);
};

class CVertexDef
{
	string m_definitionId;
	CInputDef m_inputDefinition;
public:
	CVertexDef();
	~CVertexDef();
	string id() { return m_definitionId; }
	void load(tinyxml2::XMLElement* pNode);
	string getSourceName(string semantic);
};

class CSource
{
	CGeometry* m_pParent = 0;
	char* m_pFloatArrayAsString= nullptr;
	int m_valueCount=0;
	int m_elementStride = 0;
	int m_numElements= 0;
	string m_id, m_arrayId;
	
	//map: element id->offset in array
	map<string,int> m_sourceAccesors;

	void loadAccesor(tinyxml2::XMLElement* pNode);
	void loadArray(tinyxml2::XMLElement* pNode);
public:
	CSource(CGeometry* pParent);
	~CSource();
	string getId() { return m_id; }
	int getNumElements() { return m_numElements; }
	int getNumValues() { return m_valueCount; }
	const char* getAsString() { return m_pFloatArrayAsString; }

	void load(tinyxml2::XMLElement* pNode);
};

class CGeometry
{
	CColladaModel* m_pParent;
	vector<CSource*> m_sources;
	CVertexDef m_vertexDef;
public:
	CGeometry(CColladaModel* pParent);
	~CGeometry();
	void load(tinyxml2::XMLElement* pRoot, tinyxml2::XMLElement* pNode, Matrix44& nodeTransform);

	CSource* getSource(string inputName, string requestedSemantic);
};

class CColladaModel: public CGraphicObject
{
	string m_path;

	void loadFromFile(const char* file);

	const char* findMaterialFxName(tinyxml2::XMLElement* pRootNode, string fxMaterial);
	int loadTexture(tinyxml2::XMLElement* pRootNode, tinyxml2::XMLElement* pFxProfile, string textureName);
	const char* findTextureRedirection(tinyxml2::XMLElement* pFxProfile, string textureName);
	const char* findTexture(tinyxml2::XMLElement* pRootNode,string textureName);
	void loadMaterialProperties(tinyxml2::XMLElement* pRootNode, const char* fxName, CSimpleTLMaterial* pMaterial);
	void instanceGeometry(tinyxml2::XMLElement* pRootNode, const char* geometryId, Matrix44& nodeTransform);
	void traverseVisualScene(tinyxml2::XMLElement* pRootNode, tinyxml2::XMLElement* pNode, Matrix44& matrix);
	void loadVisualScenes(tinyxml2::XMLElement* pRootNode);
	void loadSkin(tinyxml2::XMLElement* pRootNode);
public:
	CColladaModel(tinyxml2::XMLElement* pNode);
	virtual ~CColladaModel();

	CSimpleTLMaterial* loadMaterial(tinyxml2::XMLElement* pNode, string materialName);
	CMesh* loadMesh(tinyxml2::XMLElement* pRoot, tinyxml2::XMLElement* pNode, CGeometry* pGeometry, Matrix44& nodeTransform);
};

