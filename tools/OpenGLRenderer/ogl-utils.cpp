#include "stdafx.h"
#include "ogl-utils.h"
#include <gl\GL.h>
#include "../GeometryLib/bounding-box.h"

void drawBoundingBox3D(BoundingBox3D& box)
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

void drawBoundingBox2D(BoundingBox2D& box)
{
	glBegin(GL_LINES);
	glVertex2d(box.min().x(), box.min().y()); glVertex2d(box.max().x(), box.min().y());
	glVertex2d(box.max().x(), box.min().y()); glVertex2d(box.max().x(), box.max().y());
	glVertex2d(box.max().x(), box.max().y()); glVertex2d(box.min().x(), box.max().y());
	glVertex2d(box.min().x(), box.max().y()); glVertex2d(box.min().x(), box.min().y());
	glEnd();
}