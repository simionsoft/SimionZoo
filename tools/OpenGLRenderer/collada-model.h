#pragma once
#include "graphic-object-3d.h"
#include "../GeometryLib/vector3d.h"
#include <vector>
#include <string>
#include <map>

namespace tinyxml2 { class XMLElement; }
class ColladaModel;
class Geometry;
class Source;
class SimpleTLMaterial;
class Mesh;

using namespace std;


struct InputDef
{
	string semantic;
	int offset = 0;
	string source;
};

class MultiInputDef
{
	vector<InputDef> m_definitions;

public:
	MultiInputDef();
	~MultiInputDef();
	void load(tinyxml2::XMLElement* pNode);

	string getSourceName(string semantic);
	bool defines(string semantic);
};

class VertexDef
{
	string m_definitionId;
	MultiInputDef m_inputDefinition;
public:
	VertexDef();
	~VertexDef();
	string id() { return m_definitionId; }
	void load(tinyxml2::XMLElement* pNode);
	string getSourceName(string semantic);
};

class Source
{
	Geometry* m_pParent = 0;
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
	Source(Geometry* pParent);
	~Source();
	string getId() { return m_id; }
	int getNumElements() { return m_numElements; }
	int getNumValues() { return m_valueCount; }
	const char* getAsString() { return m_pFloatArrayAsString; }

	void load(tinyxml2::XMLElement* pNode);
};

class Geometry
{
	ColladaModel* m_pParent;
	vector<Source*> m_sources;
	VertexDef m_vertexDef;
public:
	Geometry(ColladaModel* pParent);
	~Geometry();
	void load(tinyxml2::XMLElement* pRoot, tinyxml2::XMLElement* pNode, Matrix44& nodeTransform);

	Source* getSource(string inputName, string requestedSemantic);
};

class ColladaModel: public GraphicObject3D
{
	string m_path;

	void loadFromFile(const char* file);

	const char* findMaterialFxName(tinyxml2::XMLElement* pRootNode, string fxMaterial);
	int loadTexture(tinyxml2::XMLElement* pRootNode, tinyxml2::XMLElement* pFxProfile, string textureName);
	const char* findTextureRedirection(tinyxml2::XMLElement* pFxProfile, string textureName);
	const char* findTexture(tinyxml2::XMLElement* pRootNode,string textureName);
	void loadMaterialProperties(tinyxml2::XMLElement* pRootNode, const char* fxName, SimpleTLMaterial* pMaterial);
	void instanceGeometry(tinyxml2::XMLElement* pRootNode, const char* geometryId, Matrix44& nodeTransform);
	void traverseVisualScene(tinyxml2::XMLElement* pRootNode, tinyxml2::XMLElement* pNode, Matrix44& matrix);
	void loadVisualScenes(tinyxml2::XMLElement* pRootNode);
	void loadSkin(tinyxml2::XMLElement* pRootNode);
public:
	ColladaModel(tinyxml2::XMLElement* pNode);
	virtual ~ColladaModel();

	SimpleTLMaterial* loadMaterial(tinyxml2::XMLElement* pNode, string materialName);
	Mesh* loadMesh(tinyxml2::XMLElement* pRoot, tinyxml2::XMLElement* pNode, Geometry* pGeometry, Matrix44& nodeTransform);
};

