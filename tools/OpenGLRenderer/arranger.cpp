#include "stdafx.h"
#include "arranger.h"
#include "graphic-object-2d.h"
#include "renderer.h"
#include "text.h"
#include "../../tools/GeometryLib/vector2d.h"
#include <algorithm>
#include <math.h>

Arranger::Arranger()
{
}


Arranger::~Arranger()
{
}

void Arranger::arrange2DObjects(vector<GraphicObject2D*>& objects
	, const Vector2D& areaOrigin, const Vector2D& areaSize, const Vector2D& minObjSize
	, const Vector2D& maxObjSize, const Vector2D& margin)
{

	if (objects.size() == 0)
		return;

	Vector2D limitedMargin = margin;

	double numObjects = (double) objects.size();
	Vector2D objSize;
	
	objSize.setX( std::max(minObjSize.x(), std::min(maxObjSize.x(), 1.0 / numObjects)) );
	
	//keep objects as separate from each other as possible
	if ((objSize.x() + limitedMargin.x())*numObjects < areaSize.x())
		limitedMargin.setX( (areaSize.x() - objSize.x()*numObjects) / (2*numObjects));
		
	double totalSizeInX = (objSize.x() + 2 * limitedMargin.x()) * numObjects;
	double numRows = ceil(totalSizeInX);

	objSize.setY( std::max(minObjSize.y(), std::min(maxObjSize.y(), 1.0 / numRows)) );

	if ((objSize.y() + limitedMargin.y())*numRows > areaSize.y())
		objSize.setY((areaSize.y() - 2*limitedMargin.y()*numRows) / numRows);

	Vector2D objOrigin;
	if (numRows == 1)
		objOrigin = Vector2D((1 - totalSizeInX) *0.5 + limitedMargin.x(), limitedMargin.y());
	else
		objOrigin = limitedMargin;

	double lastRow = 0.0;
	unsigned int viewIndex = 0;
	for (GraphicObject2D* pObj : objects)
	{
		//new row??
		if ((objSize.x() + limitedMargin.x() * 2.0) *(double)(viewIndex + 1) - lastRow >1.01)
		{
			//shift the origin y coordinate down
			objOrigin.setX(limitedMargin.x());
			objOrigin.setY(objOrigin.y() + objSize.y() + 2 * limitedMargin.y());
			lastRow++;
		}

		pObj->getTransform().setTranslation(objOrigin);
		pObj->getTransform().setScale(objSize);

		//shift the origin x coordinate for the next object to the right
		objOrigin.setX(objOrigin.x() + objSize.x() + 2 * limitedMargin.x());

		viewIndex++;
	}
}

void Arranger::tag2DObjects(vector<GraphicObject2D*>& objects, ViewPort* pViewPort)
{
	//set legend to each function viewer
	int objIndex = 0;
	Vector2D legendOffset;
	for (GraphicObject2D* pObj : objects)
	{
		legendOffset = Vector2D(0.0, pObj->getTransform().scale().y() + 0.01);
		Text2D* pFunctionLegend = new Text2D(pObj->name() + string("-legend"), pObj->getTransform().translation() + legendOffset, 0.3);
		pFunctionLegend->set(pObj->name());
		Renderer::get()->add2DGraphicObject(pFunctionLegend, pViewPort);

		objIndex++;
	}
}