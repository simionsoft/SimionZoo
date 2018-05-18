#include "stdafx.h"
#include "arranger.h"
#include "graphic-object-2d.h"
#include "renderer.h"
#include "text.h"
#include "../../tools/GeometryLib/vector2d.h"
#include <algorithm>

Arranger::Arranger()
{
}


Arranger::~Arranger()
{
}

void Arranger::arrange2DObjects(vector<GraphicObject2D*>& objects, Vector2D& areaOrigin, Vector2D& areaSize, Vector2D& minObjSize, Vector2D& maxObjSize, Vector2D& margin)
{

	if (objects.size() == 0)
		return;

	size_t numObjects = objects.size();
	double dueSize = 1.0 / numObjects;
	Vector2D objSize;
	
	objSize.setX( std::max(minObjSize.x(), std::min(maxObjSize.x(), 1.0 / numObjects)) );
		
	double totalSizeInX = (objSize.x() + 2 * margin.x()) * numObjects;
	double numRows = ceil(totalSizeInX);

	objSize.setY( std::max(minObjSize.y(), std::min(maxObjSize.y(), 1.0 / numRows)) );

	Vector2D objOrigin;
	if (numRows == 1)
		objOrigin = Vector2D((1 - totalSizeInX) *0.5 + margin.x(), margin.y());
	else
		objOrigin = margin;
	//Vector2D functionViewSize = Vector2D(objSize - 2 * xOffset, viewSizeInY - 2 * yOffset);
	objSize = objSize - margin * 2.0;

	unsigned int lastRow = 0;
	unsigned int viewIndex = 0;
	for each (GraphicObject2D* pObj in objects)
	{
		//new row??
		if ((unsigned int)floor((objSize.x() + margin.x() * 2.0) *(double)(viewIndex + 1)) != lastRow)
		{
			//shift the origin y coordinate down
			objOrigin.setX(margin.x());
			objOrigin.setY(objOrigin.y() + objSize.y() + 2 * margin.y());
			lastRow++;
		}

		pObj->getTransform().setTranslation(objOrigin);
		pObj->getTransform().setScale(objSize);

		//shift the origin x coordinate for the next object to the right
		objOrigin.setX(objOrigin.x() + objSize.x() + 2 * margin.x());

		viewIndex++;
	}
}

void Arranger::tag2DObjects(vector<GraphicObject2D*>& objects, ViewPort* pViewPort)
{
	//set legend to each function viewer
	int objIndex = 0;
	Vector2D legendOffset;
	for each (GraphicObject2D* pObj in objects)
	{
		legendOffset = Vector2D(0.0, pObj->getTransform().scale().y() + 0.01);
		Text2D* pFunctionLegend = new Text2D(pObj->name() + string("-legend"), pObj->getTransform().translation() + legendOffset, 0.3);
		pFunctionLegend->set(pObj->name());
		Renderer::get()->add2DGraphicObject(pFunctionLegend, pViewPort);

		objIndex++;
	}
}