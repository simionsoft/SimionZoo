#pragma once

#include <string>
#include "scene-actor.h"
#include "../GeometryLib/bounding-box.h"
class Mesh;
class Material;
class XMLElement;
class BoundingCylinder;
using namespace std;
#include <vector>

class GraphicObject: public SceneActor
{
protected:
	string m_name;
	vector<Mesh*> m_meshes;
	BoundingBox3D m_bb;

	void updateBoundingBox();
public:
	GraphicObject(string name);
	GraphicObject(tinyxml2::XMLElement* pNode);
	virtual ~GraphicObject();

	string name() { return m_name; }

	static GraphicObject* getInstance(tinyxml2::XMLElement* pNode);

	void addMesh(Mesh* pMesh) { m_meshes.push_back(pMesh); }

	void draw();

	BoundingBox3D boundingBox();

	void fitToBoundingBox(BoundingBox3D* newBB);
	void fitToBoundingCylinder(BoundingCylinder* newBC);
};

