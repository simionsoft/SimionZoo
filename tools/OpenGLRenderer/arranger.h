#pragma once

#include <vector>
using namespace std;

class GraphicObject2D;
class Vector2D;
class ViewPort;

class Arranger
{
public:
	Arranger();
	virtual ~Arranger();

	void arrange2DObjects(vector<GraphicObject2D*>& objects, const Vector2D& areaOrigin,const  Vector2D& areaSize
		, const Vector2D& minObjSize, const Vector2D& maxObjSize, const Vector2D& margin );
	void tag2DObjects(vector<GraphicObject2D*>& objects, ViewPort* pViewPort);
};

