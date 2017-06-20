#include "stdafx.h"
#include "Camera.h"
#include "xml-load-utils.h"
#include "vector3d.h"
#include "matrix33.h"
#include "transform3d.h"
#include "renderer.h"

CCamera::CCamera(): CSceneActor()
{
}

CCamera::~CCamera()
{
}

Matrix44 CCamera::getModelviewMatrix() const
{
	Matrix44 mat, rot, trans;
	rot = Matrix44::rotationMatrix(m_transform.rotation().inverse());
	trans = Matrix44::translationMatrix(m_transform.translation().inverse());
	mat = rot*trans;

	return mat;
}

CCamera* CCamera::getInstance(tinyxml2::XMLElement* pNode)
{
	tinyxml2::XMLElement* pChild = pNode->FirstChildElement();
	if (!strcmp(pChild->Name(), XML_TAG_SIMPLE_CAMERA))
		return new CSimpleCamera(pChild);
	return nullptr;
}

CSimpleCamera::CSimpleCamera(tinyxml2::XMLElement* pNode)
{
	tinyxml2::XMLElement* pChild = pNode->FirstChildElement(XML_TAG_TRANSFORM);
	if (pChild)
		m_transform.load(pChild);
}

CSimpleCamera::CSimpleCamera()
{
}

void CSimpleCamera::set()
{
	//set projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1.0, 1.0, -1.0, 1.0, nearPlane, farPlane);

	//set modelview matrix
	glMatrixMode(GL_MODELVIEW);
	Matrix44 matrix = getModelviewMatrix();
	glLoadMatrixd(matrix.asArray());
}

void CCamera::set2DView()
{
	int sizeX, sizeY;
	//set projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	CRenderer::get()->getWindowsSize(sizeX, sizeY);
	gluOrtho2D(0.0, sizeX, 0.0, sizeY);

	//set modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}