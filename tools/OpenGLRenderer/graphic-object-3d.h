#pragma once

#include <string>
#include "scene-actor-3d.h"
#include "../GeometryLib/bounding-box.h"
class Mesh;
class Material;
class XMLElement;
class BoundingCylinder;
using namespace std;
#include <vector>

class GraphicObject3D: public SceneActor3D
{
protected:
	string m_name;
	vector<Mesh*> m_meshes;
	BoundingBox3D m_bb;

	vector<GraphicObject3D*> m_children;

	void updateBoundingBox();
public:
	GraphicObject3D(string name);
	GraphicObject3D(tinyxml2::XMLElement* pNode);
	virtual ~GraphicObject3D();

	string name() { return m_name; }

	static GraphicObject3D* getInstance(tinyxml2::XMLElement* pNode);

	void drawChildren();
	void addChild(GraphicObject3D* child);
	vector<GraphicObject3D*>& getChildren();

	void addMesh(Mesh* pMesh) { m_meshes.push_back(pMesh); }

	void draw();

	BoundingBox3D boundingBox();

	void fitToBoundingBox(BoundingBox3D* newBB);
	void fitToBoundingCylinder(BoundingCylinder* newBC);
};

