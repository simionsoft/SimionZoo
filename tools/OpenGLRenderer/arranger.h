#pragma once

#include <vector>
using namespace std;

class GraphicObject2D;
class Vector2D;

class Arranger
{
public:
	Arranger();
	virtual ~Arranger();

	void arrangeObjects2D(vector<GraphicObject2D*>& objects, Vector2D& areaOrigin, Vector2D& areaSize
		, Vector2D& minObjSize, Vector2D& maxObjSize, Vector2D& margin );
};

