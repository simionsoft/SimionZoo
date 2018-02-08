#include "stdafx.h"
#include "graphic-object-2d.h"
#include "material.h"
#include "renderer.h"
#include "text.h"

CGraphicObject2D::CGraphicObject2D(string name)
{
	m_name = name;
}


CGraphicObject2D::~CGraphicObject2D()
{
}



//void CGraphicObject2D::setTransform()
//{
//	glMatrixMode(GL_MODELVIEW);
//	glPushMatrix();
//	double* pMatrix = m_transform.getOpenGLMatrix();
//	glMultMatrixd(pMatrix);
//}
//
//void CGraphicObject2D::restoreTransform()
//{
//	glPopMatrix();
//}




C2DMeter::C2DMeter(string name, Vector2D origin, Vector2D size, int depth): CGraphicObject2D(name)
{
	int screenWidth, screenHeight;

	CRenderer::get()->getWindowsSize(screenWidth, screenHeight);
	
	m_transform.setTranslation(Vector2D(origin.x()*screenWidth, origin.y()*screenHeight));
	m_transform.setScale(Vector2D(size.x()*screenWidth, size.y()*screenHeight));
	m_transform.setDepth(depth);

	m_pMaterial= new CColorMaterial();

	m_pText = new C2DText(name + "/text" , origin, depth + 1); //over the progress bar
	m_pText->addPixelOffset(Vector2D(2, 5));
}

C2DMeter::~C2DMeter()
{
	if (m_pMaterial != nullptr) delete m_pMaterial;
	delete m_pText;
}


void C2DMeter::draw()
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

void C2DMeter::setValue(double value)
{
	m_value = m_valueRange.clamp(value);

	double normValue= m_valueRange.normPosInRange(m_value);

	//udpate the color
	((CColorMaterial*)m_pMaterial)->setColor(Color(1.0, 0.0, 0.0, 1.0).lerp(Color(0.0, 1.0, 0.0, 1.0), normValue));
	//update the text
	char buffer[1024];
	sprintf_s(buffer, 1024, "%s: %.4f", m_name.c_str(), value);
	m_pText->set(string(buffer));
}

BoundingBox2D& CGraphicObject2D::boundingBox()
{
	return m_bb;
}