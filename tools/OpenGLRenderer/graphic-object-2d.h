#pragma once

#include "../GeometryLib/transform2d.h"
#include "../GeometryLib/vector2d.h"
#include "../GeometryLib/bounding-box.h"
#include "scene-actor-2d.h"
#include <string>
#include <vector>
using namespace std;
class Material;
class Text2D;

class GraphicObject2D: public SceneActor2D
{
protected:
	string m_name;
	BoundingBox2D m_bb;
	vector<GraphicObject2D*> m_children;
public:
	GraphicObject2D(string name);
	GraphicObject2D(string name, Vector2D origin, Vector2D size, double rotation, double depth = 0.0);
	virtual ~GraphicObject2D();

	GraphicObject2D(tinyxml2::XMLElement* pNode);
	static GraphicObject2D* getInstance(tinyxml2::XMLElement* pNode);

	virtual void draw() = 0;

	void drawChildren();
	void addChild(GraphicObject2D* child);
	vector<GraphicObject2D*>& getChildren();

	string name() { return m_name; }

	BoundingBox2D& boundingBox();
};

