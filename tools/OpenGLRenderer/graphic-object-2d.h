#pragma once

#include "../GeometryLib/transform2d.h"
#include "../GeometryLib/vector2d.h"
#include "../GeometryLib/bounding-box.h"
#include "scene-actor-2d.h"
#include <string>
using namespace std;
class Material;
class Text2D;

class GraphicObject2D: public SceneActor2D
{
protected:
	string m_name;
	Transform2D m_transform;
	BoundingBox2D m_bb;
public:
	GraphicObject2D(string name);
	GraphicObject2D(string name, Vector2D origin, Vector2D size, int depth = 0);
	virtual ~GraphicObject2D();

	GraphicObject2D(tinyxml2::XMLElement* pNode);
	static GraphicObject2D* getInstance(tinyxml2::XMLElement* pNode);

	virtual void draw() = 0;

	string name() { return m_name; }

	BoundingBox2D& boundingBox();
};

