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
	for (Bindable* object : boundObjects)
		delete object;
}

void Binding::update(double value)
{
	for (Bindable* object : boundObjects)
		object->update( value*m_multiplier + m_offset );
}

void Binding::addBoundObject(Bindable* pObj)
{
	boundObjects.push_back(pObj);
}