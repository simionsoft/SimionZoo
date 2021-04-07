#pragma once

#include "../GeometryLib/bounding-box.h"
#include "../GeometryLib/matrix44.h"
#include "../GeometryLib/vector3d.h"
#include "../GeometryLib/vector2d.h"
class Material;
class ColladaModel;
class Geometry;
namespace tinyxml2 { class XMLElement; }
#include <string>
#include <vector>
using namespace std;

class Mesh
{
	unsigned int m_primitiveType = GL_TRIANGLES;

	vector<unsigned int> m_pIndices;
	vector<unsigned int> m_pVerticesPerPolygon;
	unsigned int m_numIndicesPerVertex = 1;
	//vertices
	unsigned int m_posOffset = 0;
	Point3D* m_pPositions = 0;
	unsigned int m_numPositions = 0;

	unsigned int m_normalOffset = 0;
	Vector3D* m_pNormals = 0;
	unsigned int m_numNormals = 0;

	unsigned int m_texCoordOffset = 0;
	Vector2D* m_pTexCoords = 0;
	unsigned int m_numTexCoords = 0;

	//material
	Material* m_pMaterial = 0;

public:
	Mesh();
	~Mesh();

	void draw();

	void setMaterial(Material* pMaterial) { m_pMaterial = pMaterial; }
	void updateBoundingBox(BoundingBox3D& bb);
	void transformVertices(Vector3D& translation, Vector3D& scale);

	//dummy versions for now
	void allocPositions(unsigned int numElements) { m_pPositions = new Point3D[numElements]; m_numPositions = numElements; }
	void allocNormals(unsigned int numElements) { m_pNormals = new Vector3D[numElements]; m_numNormals = numElements;}
	void allocTexCoords(unsigned int numElements) { m_pTexCoords = new Vector2D[numElements]; m_numTexCoords = numElements;}

	Point3D* getPosArray() { return m_pPositions; }
	Vector3D* getNormalArray() { return m_pNormals; }
	Vector2D* getTexCoordArray() { return m_pTexCoords; }
	
	unsigned int getNumPositions() const{ return m_numPositions; }
	Point3D& getPosition(unsigned int i) { return m_pPositions[i]; }
	unsigned int getNumNormals() const { return m_numNormals; }
	Vector3D& getNormal(unsigned int i) { return m_pNormals[i]; }
	unsigned int getNumTexCoords() const { return m_numTexCoords; }
	Vector2D& getTexCoord(unsigned int i) const { return m_pTexCoords[i]; }

	void setPosOffset(unsigned int offset) { m_posOffset = offset; }
	void setNormalOffset(unsigned int offset) { m_normalOffset = offset; }
	void setTexCoordOffset(unsigned int offset) { m_texCoordOffset = offset; }
	void setNumIndicesPerVertex(unsigned int numIndices) { m_numIndicesPerVertex = numIndices; }
	
	vector<unsigned int>& getIndexArray() { return m_pIndices; }
	int getNumIndices() const { return m_pIndices.size(); }
	vector<unsigned int>& getVertexCountArray() { return m_pVerticesPerPolygon; }

	void flipYZAxis();
	void flipVTexCoord();
	void reorderIndices();

	void setPrimitiveType(unsigned int type) { m_primitiveType = type; }

	void allocIndices(unsigned int numIndices) { m_pIndices = vector<unsigned int>(numIndices); }

	void ignoreTexCoordsAndNormals();
};
