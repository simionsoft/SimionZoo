#include "stdafx.h"
#include "bounding-box.h"

//{0, 1, 2,
//3, 2, 1,
//4, 0, 6,
//6, 0, 2,
//5, 1, 4,
//4, 1, 0,
//7, 3, 1,
//7, 1, 5,
//5, 4, 7,
//7, 4, 6,
//7, 2, 3,
//7, 6, 2 };
//Vector3D vertices[8] =
//{
//Vector3D(-0.5,-0.5,-0.5) min
//Vector3D(0.5,0.5,0.5) max

//	Vector3D(0.5,0.5,0.5),
//	Vector3D(-0.5,0.5,0.5),
//	Vector3D(0.5,-0.5,0.5),
//	Vector3D(-0.5,-0.5,0.5),
//	Vector3D(0.5,0.5,-0.5),
//	Vector3D(-0.5,0.5,-0.5),
//	Vector3D(0.5,-0.5,-0.5),
//	Vector3D(-0.5,-0.5,-0.5) };

void BoundingBox3D::draw() const
{
	glBegin(GL_LINES);
	//FRONT
	glVertex3d(m_min.x(), m_min.y(), m_max.z());
	glVertex3d(m_max.x(), m_min.y(), m_max.z());
	glVertex3d(m_max.x(), m_min.y(), m_max.z());
	glVertex3d(m_max.x(), m_max.y(), m_max.z());
	glVertex3d(m_max.x(), m_max.y(), m_max.z());
	glVertex3d(m_min.x(), m_max.y(), m_max.z());
	glVertex3d(m_min.x(), m_max.y(), m_max.z());
	glVertex3d(m_min.x(), m_min.y(), m_max.z());
	//BACK
	glVertex3d(m_min.x(), m_min.y(), m_min.z());
	glVertex3d(m_max.x(), m_min.y(), m_min.z());
	glVertex3d(m_max.x(), m_min.y(), m_min.z());
	glVertex3d(m_max.x(), m_max.y(), m_min.z());
	glVertex3d(m_max.x(), m_max.y(), m_min.z());
	glVertex3d(m_min.x(), m_max.y(), m_min.z());
	glVertex3d(m_min.x(), m_max.y(), m_min.z());
	glVertex3d(m_min.x(), m_min.y(), m_min.z());
	//4 lines between front face and back face
	glVertex3d(m_min.x(), m_min.y(), m_min.z());
	glVertex3d(m_min.x(), m_min.y(), m_max.z());
	glVertex3d(m_max.x(), m_min.y(), m_min.z());
	glVertex3d(m_max.x(), m_min.y(), m_max.z());
	glVertex3d(m_max.x(), m_max.y(), m_min.z());
	glVertex3d(m_max.x(), m_max.y(), m_max.z());
	glVertex3d(m_min.x(), m_max.y(), m_min.z());
	glVertex3d(m_min.x(), m_max.y(), m_max.z());
	glEnd();
}

void BoundingBox2D::draw() const
{
	glBegin(GL_LINES);
	glVertex2d(m_min.x(), m_min.y()); glVertex2d(m_max.x(), m_min.y());
	glVertex2d(m_max.x(), m_min.y()); glVertex2d(m_max.x(), m_max.y());
	glVertex2d(m_max.x(), m_max.y()); glVertex2d(m_min.x(), m_max.y());
	glVertex2d(m_min.x(), m_max.y()); glVertex2d(m_min.x(), m_min.y());
	glEnd();
}