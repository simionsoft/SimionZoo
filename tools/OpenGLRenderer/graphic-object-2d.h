#pragma once

#include "../GeometryLib/transform2d.h"
#include "../GeometryLib/vector2d.h"
#include "../GeometryLib/bounding-box.h"
#include <string>
using namespace std;
class Material;
class Text2D;

class GraphicObject2D
{
protected:
	string m_name;
	Transform2D m_transform;
	BoundingBox2D m_bb;
public:
	GraphicObject2D(string name);
	virtual ~GraphicObject2D();

	string name() { return m_name; }

	virtual void draw() = 0;
	BoundingBox2D& boundingBox();
};

class Meter2D: public GraphicObject2D
{
	Material* m_pMaterial;
	Range m_valueRange;
	double m_value;
	Text2D *m_pText;
public:
	//the constructor uses normalized coordinates [0,1]
	Meter2D(string name, Vector2D origin, Vector2D size, int depth= 0);
	virtual ~Meter2D();

	void setValueRange(Range range) { m_valueRange = range; }
	void setValue(double value);
	void draw();
};