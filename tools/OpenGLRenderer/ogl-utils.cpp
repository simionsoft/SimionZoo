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
#include "ogl-utils.h"
#include "../../3rd-party/glew2/include/glew.h"
#include "../GeometryLib/bounding-box.h"

void drawBoundingBox3D(const BoundingBox3D& box)
{
	glBegin(GL_LINES);
	//FRONT
	glVertex3d(box.min().x(), box.min().y(), box.max().z());
	glVertex3d(box.max().x(), box.min().y(), box.max().z());
	glVertex3d(box.max().x(), box.min().y(), box.max().z());
	glVertex3d(box.max().x(), box.max().y(), box.max().z());
	glVertex3d(box.max().x(), box.max().y(), box.max().z());
	glVertex3d(box.min().x(), box.max().y(), box.max().z());
	glVertex3d(box.min().x(), box.max().y(), box.max().z());
	glVertex3d(box.min().x(), box.min().y(), box.max().z());
	//BACK
	glVertex3d(box.min().x(), box.min().y(), box.min().z());
	glVertex3d(box.max().x(), box.min().y(), box.min().z());
	glVertex3d(box.max().x(), box.min().y(), box.min().z());
	glVertex3d(box.max().x(), box.max().y(), box.min().z());
	glVertex3d(box.max().x(), box.max().y(), box.min().z());
	glVertex3d(box.min().x(), box.max().y(), box.min().z());
	glVertex3d(box.min().x(), box.max().y(), box.min().z());
	glVertex3d(box.min().x(), box.min().y(), box.min().z());
	//4 lines between front face and back face
	glVertex3d(box.min().x(), box.min().y(), box.min().z());
	glVertex3d(box.min().x(), box.min().y(), box.max().z());
	glVertex3d(box.max().x(), box.min().y(), box.min().z());
	glVertex3d(box.max().x(), box.min().y(), box.max().z());
	glVertex3d(box.max().x(), box.max().y(), box.min().z());
	glVertex3d(box.max().x(), box.max().y(), box.max().z());
	glVertex3d(box.min().x(), box.max().y(), box.min().z());
	glVertex3d(box.min().x(), box.max().y(), box.max().z());
	glEnd();
}

void drawBoundingBox2D(const BoundingBox2D& box)
{
	glBegin(GL_LINES);
	glVertex2d(box.min().x(), box.min().y()); glVertex2d(box.max().x(), box.min().y());
	glVertex2d(box.max().x(), box.min().y()); glVertex2d(box.max().x(), box.max().y());
	glVertex2d(box.max().x(), box.max().y()); glVertex2d(box.min().x(), box.max().y());
	glVertex2d(box.min().x(), box.max().y()); glVertex2d(box.min().x(), box.min().y());
	glEnd();
}