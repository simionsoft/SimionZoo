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
	for each (Bindable* object in boundObjects)
		delete object;
}

void Binding::update(double value)
{
	for each (Bindable* object in boundObjects)
		object->update((value + m_offset) *m_multiplier);
}

void Binding::addBoundObject(Bindable* pObj)
{
	boundObjects.push_back(pObj);
}