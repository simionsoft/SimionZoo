/*
	SimionZoo: A framework for online model-free Reinforcement Learning on continuous
	control problems

	Copyright (c) 2016 SimionSoft. https://github.com/simionsoft

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

#include "stdafx.h"
#include "viewport.h"
#include "camera.h"
#include "light.h"
#include "graphic-object-3d.h"
#include "graphic-object-2d.h"
#include "ogl-utils.h"
#include "renderer.h"
#include "../GeometryLib/frustum.h"

ViewPort::ViewPort(double minNormScreenX, double minNormScreenY, double maxNormScreenX, double maxNormScreenY)
{
	resize(minNormScreenX, minNormScreenY, maxNormScreenX, maxNormScreenY);
}

void ViewPort::resize(double minNormScreenX, double minNormScreenY, double maxNormScreenX, double maxNormScreenY)
{
	int windowWidth, windowHeight;
	Renderer::get()->getWindowsSize(windowWidth, windowHeight);

	m_minScreenX = (unsigned int)(minNormScreenX * (double) windowWidth);
	m_minScreenY = (unsigned int)(minNormScreenY * (double) windowHeight);
	m_maxScreenX = (unsigned int)(maxNormScreenX * (double) (windowWidth - 1));
	m_maxScreenY = (unsigned int)(maxNormScreenY * (double) (windowHeight - 1));
}

ViewPort::~ViewPort()
{
}

void ViewPort::drawAxes()
{
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_LINES);
	glColor3f(1.f, 0.f, 0.f);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(1.f, 0.0, 0.f);
	glColor3f(0.f, 1.f, 0.f);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.f, 1.0, 0.f);
	glColor3f(0.f, 0.f, 1.f);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.f, 0.0, 1.f);
	glEnd();
	glEnable(GL_LIGHTING);
}

int ViewPort::draw()
{
	int numObjectsDrawn = 0;
	glViewport(m_minScreenX, m_minScreenY, m_maxScreenX - m_minScreenX, m_maxScreenY - m_minScreenY);

	//set 3d view
	if (m_pActiveCamera == nullptr)
		m_pActiveCamera = Renderer::get()->getActiveCamera();

	m_pActiveCamera->set();
	//set lights
	Light::enable(true);

	for (auto it = m_lights.begin(); it != m_lights.end(); ++it)
	{
		(*it)->set();
	}
	//draw 3d objects in the scene
	Frustum& frustum = m_pActiveCamera->getFrustum();

#ifdef _DEBUG
	if (m_3DgraphicObjects.size()>0)
		drawAxes();
#endif

	for (auto object : m_3DgraphicObjects)
	{
		if (frustum.isVisible(object->boundingBox()))
		{
			object->draw();

			if (m_bDrawBoundingBoxes)
			{
				object->setTransform();
				drawBoundingBox3D(object->boundingBox());
				object->restoreTransform();
			}
			++numObjectsDrawn;
		}
	}
	Light::enable(false);
	//set 2d view
	if (m_pActiveCamera) m_pActiveCamera->set2DView();
	//draw 2d objects
	for (auto object : m_2DgraphicObjects)
	{
		object->setTransform();

		object->draw();
		++numObjectsDrawn;

		if (m_bDrawBoundingBoxes)
			drawBoundingBox2D(object->boundingBox());

		object->restoreTransform();
	}
	return numObjectsDrawn;
}