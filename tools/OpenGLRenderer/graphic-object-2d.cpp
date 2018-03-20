#include "stdafx.h"
#include "graphic-object-2d.h"
#include "material.h"
#include "renderer.h"
#include "text.h"

GraphicObject2D::GraphicObject2D(string name)
{
	m_name = name;
}


GraphicObject2D::~GraphicObject2D()
{
}



//void GraphicObject2D::setTransform()
//{
//	glMatrixMode(GL_MODELVIEW);
//	glPushMatrix();
//	double* pMatrix = m_transform.getOpenGLMatrix();
//	glMultMatrixd(pMatrix);
//}
//
//void GraphicObject2D::restoreTransform()
//{
//	glPopMatrix();
//}




Meter2D::Meter2D(string name, Vector2D origin, Vector2D size, int depth): GraphicObject2D(name)
{
	int screenWidth, screenHeight;

	Renderer::get()->getWindowsSize(screenWidth, screenHeight);
	
	m_transform.setTranslation(Vector2D(origin.x()*screenWidth, origin.y()*screenHeight));
	m_transform.setScale(Vector2D(size.x()*screenWidth, size.y()*screenHeight));
	m_transform.setDepth(depth);

	m_pMaterial= new ColorMaterial();

	m_pText = new Text2D(name + "/text" , origin, depth + 1); //over the progress bar
	m_pText->addPixelOffset(Vector2D(2, 5));
}

Meter2D::~Meter2D()
{
	if (m_pMaterial != nullptr) delete m_pMaterial;
	delete m_pText;
}


void Meter2D::draw()
{
	double normValue = m_valueRange.normPosInRange(m_value);
	Vector2D bottomLeft= m_transform.translation();
	Vector2D topRight = bottomLeft + m_transform.scale();

	glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_LINE_STRIP);
	glVertex3d(bottomLeft.x(), bottomLeft.y(), m_transform.depth());
	glVertex3d(topRight.x(), bottomLeft.y(), m_transform.depth());
	glVertex3d(topRight.x(), topRight.y(), m_transform.depth());
	glVertex3d(bottomLeft.x(), topRight.y(), m_transform.depth());
	glVertex3d(bottomLeft.x(), bottomLeft.y(), m_transform.depth());
	glEnd();
	
	topRight = bottomLeft + Vector2D(m_transform.scale().x()*normValue, m_transform.scale().y());
	m_pMaterial->set();

	glBegin(GL_QUADS);
	glVertex3d(bottomLeft.x(), bottomLeft.y(), m_transform.depth());
	glVertex3d(topRight.x(), bottomLeft.y(), m_transform.depth());
	glVertex3d(topRight.x(), topRight.y(), m_transform.depth());
	glVertex3d(bottomLeft.x(), topRight.y(), m_transform.depth());
	glEnd();

	m_pText->draw();
}

void Meter2D::setValue(double value)
{
	m_value = m_valueRange.clamp(value);

	double normValue= m_valueRange.normPosInRange(m_value);

	//udpate the color
	((ColorMaterial*)m_pMaterial)->setColor(Color(1.0, 0.0, 0.0, 1.0).lerp(Color(0.0, 1.0, 0.0, 1.0), normValue));
	//update the text
	char buffer[1024];
	sprintf_s(buffer, 1024, "%s: %.4f", m_name.c_str(), value);
	m_pText->set(string(buffer));
}

BoundingBox2D& GraphicObject2D::boundingBox()
{
	return m_bb;
}