#include "stdafx.h"
#include "bindings.h"
#include "../GeometryLib/vector3d.h"
#include "../GeometryLib/vector2d.h"
#include "../GeometryLib/transform3d.h"
#include "../GeometryLib/bounding-box.h"
#include "../GeometryLib/bounding-cylinder.h"
#include "color.h"
#include "renderer.h"
#include "xml-load.h"



Binding::~Binding()
{
	for (auto it = boundObjects.begin(); it != boundObjects.end(); ++it)
		delete (*it);
}

void Binding::update(double value)
{
	for (auto it = boundObjects.begin(); it != boundObjects.end(); ++it)
		(*it)->update(value);
}

void Binding::addBoundObject(Bindable* pObj)
{
	boundObjects.push_back(pObj);
}