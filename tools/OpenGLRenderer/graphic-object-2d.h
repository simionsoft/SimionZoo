#pragma once

#include "../GeometryLib/transform2d.h"
#include "../GeometryLib/vector2d.h"
#include "../GeometryLib/bounding-box.h"
#include <string>
using namespace std;
class CMaterial;
class C2DText;

class CGraphicObject2D
{
protected:
	string m_name;
	Transform2D m_transform;
	BoundingBox2D m_bb;
public:
	CGraphicObject2D(string name);
	virtual ~CGraphicObject2D();

	string name() { return m_name; }

	virtual void draw() = 0;
	BoundingBox2D& boundingBox();
};

class C2DMeter: public CGraphicObject2D
{
	CMaterial* m_pMaterial;
	Range m_valueRange;
	double m_value;
	C2DText *m_pText;
public:
	//the constructor uses normalized coordinates [0,1]
	C2DMeter(string name, Vector2D origin, Vector2D size, int depth= 0);
	virtual ~C2DMeter();

	void setValueRange(Range range) { m_valueRange = range; }
	void setValue(double value);
	void draw();
};