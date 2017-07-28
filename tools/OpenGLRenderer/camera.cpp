#include "stdafx.h"
#include "camera.h"
#include "xml-load.h"
#include "renderer.h"
#include "../GeometryLib/vector3d.h"
#include "../GeometryLib/matrix33.h"
#include "../GeometryLib/transform3d.h"

CCamera::CCamera(): CSceneActor()
{
}

CCamera::~CCamera()
{
}

Matrix44 CCamera::getModelviewMatrix() const
{
	Matrix44 mat, rot, trans;
	rot.setRotation(m_transform.rotation().inverse());
	trans.setTranslation(m_transform.translation().inverse());
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
		XML::load(pChild,m_transform);
}

CSimpleCamera::CSimpleCamera()
{
}

void CSimpleCamera::set()
{
	//set projection matrix
	glMatrixMode(GL_PROJECTION);
	Matrix44 perspective;
	perspective.setPerspective(1.0, 1.0, nearPlane, farPlane);
	glLoadMatrixd(perspective.asArray());

	//set modelview matrix
	glMatrixMode(GL_MODELVIEW);
	Matrix44 matrix = getModelviewMatrix();
	glLoadMatrixd(matrix.asArray());

	m_frustum.fromCameraMatrix(perspective*matrix);
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