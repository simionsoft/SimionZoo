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

	void arrange2DObjects(vector<GraphicObject2D*>& objects, Vector2D& areaOrigin, Vector2D& areaSize
		, Vector2D& minObjSize, Vector2D& maxObjSize, Vector2D& margin );
	void tag2DObjects(vector<GraphicObject2D*>& objects, ViewPort* pViewPort);
};

