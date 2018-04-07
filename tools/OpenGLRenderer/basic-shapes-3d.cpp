#include "stdafx.h"
#include "basic-shapes-3d.h"
#include "xml-load.h"
#include "bindings.h"
#include "material.h"
#include "mesh.h"

BasicShape::BasicShape(tinyxml2::XMLElement* pNode) : GraphicObject3D(pNode)
{
	tinyxml2::XMLElement* pChild;
	pChild= pNode->FirstChildElement(XML_TAG_MATERIAL);
	if (pChild)
		m_pMaterialLoaded = Material::getInstance(pChild->FirstChildElement());
}

BasicShape::BasicShape(string name) : GraphicObject3D(name)
{

}

BasicShape::~BasicShape()
{}

Sphere::Sphere(tinyxml2::XMLElement* pNode): BasicShape(pNode)
{
	/*
	void GL_ShapeDrawer::drawSphere(btScalar radius, int lats, int longs)
{
	int i, j;
	for (i = 0; i <= lats; i++) {
		btScalar lat0 = SIMD_PI * (-btScalar(0.5) + (btScalar)(i - 1) / lats);
		btScalar z0 = radius*sin(lat0);
		btScalar zr0 = radius*cos(lat0);

		btScalar lat1 = SIMD_PI * (-btScalar(0.5) + (btScalar)i / lats);
		btScalar z1 = radius*sin(lat1);
		btScalar zr1 = radius*cos(lat1);

		glBegin(GL_QUAD_STRIP);
		for (j = 0; j <= longs; j++) {
			btScalar lng = 2 * SIMD_PI * (btScalar)(j - 1) / longs;
			btScalar x = cos(lng);
			btScalar y = sin(lng);
			glNormal3f(x * zr1, y * zr1, z1);
			glVertex3f(x * zr1, y * zr1, z1);
			glNormal3f(x * zr0, y * zr0, z0);
			glVertex3f(x * zr0, y * zr0, z0);
		}
		glEnd();
	}
}

	*/
}

Box::Box(tinyxml2::XMLElement* pNode) : BasicShape(pNode)
{
	bool bDrawTopSide= true;
	//box attributes
	if (pNode->Attribute(XML_TAG_BOX_DRAW_TOP)
		&& (!strcmp(pNode->Attribute(XML_TAG_BOX_DRAW_TOP), "false")
			|| !strcmp(pNode->Attribute(XML_TAG_BOX_DRAW_TOP), "False")))
		bDrawTopSide = false;

	Mesh* pMesh = new Mesh();
	//vertex positions
	pMesh->allocPositions(8);
	pMesh->getPosArray()[0] = Point3D(m_min.x(), m_min.y(),m_max.z());
	pMesh->getPosArray()[1] = Point3D(m_max.x(), m_min.y(), m_max.z());
	pMesh->getPosArray()[2] = Point3D(m_max.x(), m_max.y(), m_max.z());
	pMesh->getPosArray()[3] = Point3D(m_min.x(), m_max.y(), m_max.z());
	pMesh->getPosArray()[4] = Point3D(m_min.x(), m_min.y(), m_min.z());
	pMesh->getPosArray()[5] = Point3D(m_max.x(), m_min.y(), m_min.z());
	pMesh->getPosArray()[6] = Point3D(m_max.x(), m_max.y(), m_min.z());
	pMesh->getPosArray()[7] = Point3D(m_min.x(), m_max.y(), m_min.z());
	pMesh->allocNormals(6);
	pMesh->getNormalArray()[0] = Point3D(0.0,0.0,1.0); //front
	pMesh->getNormalArray()[1] = Point3D(0.0,0.0,-1.0); //back
	pMesh->getNormalArray()[2] = Point3D(1.0,0.0,0.0); //right
	pMesh->getNormalArray()[3] = Point3D(-1.0,0.0,0.0); //left
	pMesh->getNormalArray()[4] = Point3D(0.0,1.0,0.0); //top
	pMesh->getNormalArray()[5] = Point3D(0.0,-1.0,0.0); //bottom
	pMesh->allocTexCoords(4);
	pMesh->getTexCoordArray()[0] = Vector2D(0.0, 1.0);
	pMesh->getTexCoordArray()[1] = Vector2D(1.0, 1.0);
	pMesh->getTexCoordArray()[2] = Vector2D(1.0, 0.0);
	pMesh->getTexCoordArray()[3] = Vector2D(0.0, 0.0);
	
	if (bDrawTopSide)
		pMesh->allocIndices(24*3);
	else pMesh->allocIndices(20 * 3);
	//front
	pMesh->getIndexArray()[0] = 0; //positions
	pMesh->getIndexArray()[3] = 1;
	pMesh->getIndexArray()[6] = 2;
	pMesh->getIndexArray()[9] = 3; 
	pMesh->getIndexArray()[1] = 0;//normals
	pMesh->getIndexArray()[4] = 0;
	pMesh->getIndexArray()[7] = 0;
	pMesh->getIndexArray()[10] = 0;
	pMesh->getIndexArray()[2] = 0;//tex.coords
	pMesh->getIndexArray()[5] = 1;
	pMesh->getIndexArray()[8] = 2;
	pMesh->getIndexArray()[11] = 3;
	//back
	pMesh->getIndexArray()[12] = 5; //positions
	pMesh->getIndexArray()[15] = 4;
	pMesh->getIndexArray()[18] = 7;
	pMesh->getIndexArray()[21] = 6;
	pMesh->getIndexArray()[13] = 1;//normals
	pMesh->getIndexArray()[16] = 1;
	pMesh->getIndexArray()[19] = 1;
	pMesh->getIndexArray()[22] = 1;
	pMesh->getIndexArray()[14] = 0;//tex.coords
	pMesh->getIndexArray()[17] = 1;
	pMesh->getIndexArray()[20] = 2;
	pMesh->getIndexArray()[23] = 3;
	//right
	pMesh->getIndexArray()[24] = 1;
	pMesh->getIndexArray()[27] = 5;
	pMesh->getIndexArray()[30] = 6;
	pMesh->getIndexArray()[33] = 2;
	pMesh->getIndexArray()[25] = 2;//normals
	pMesh->getIndexArray()[28] = 2;
	pMesh->getIndexArray()[31] = 2;
	pMesh->getIndexArray()[34] = 2;
	pMesh->getIndexArray()[26] = 0;//tex.coords
	pMesh->getIndexArray()[29] = 1;
	pMesh->getIndexArray()[32] = 2;
	pMesh->getIndexArray()[35] = 3;
	//left
	pMesh->getIndexArray()[36] = 4;
	pMesh->getIndexArray()[39] = 0;
	pMesh->getIndexArray()[42] = 3;
	pMesh->getIndexArray()[45] = 7;
	pMesh->getIndexArray()[37] = 3;//normals
	pMesh->getIndexArray()[40] = 3;
	pMesh->getIndexArray()[43] = 3;
	pMesh->getIndexArray()[46] = 3;
	pMesh->getIndexArray()[38] = 0;//tex.coords
	pMesh->getIndexArray()[41] = 1;
	pMesh->getIndexArray()[44] = 2;
	pMesh->getIndexArray()[47] = 3;
	//bottom
	pMesh->getIndexArray()[48] = 0;
	pMesh->getIndexArray()[51] = 4;
	pMesh->getIndexArray()[54] = 5;
	pMesh->getIndexArray()[57] = 1;
	pMesh->getIndexArray()[49] = 5;//normals
	pMesh->getIndexArray()[52] = 5;
	pMesh->getIndexArray()[55] = 5;
	pMesh->getIndexArray()[58] = 5;
	pMesh->getIndexArray()[50] = 0;//tex.coords
	pMesh->getIndexArray()[53] = 1;
	pMesh->getIndexArray()[56] = 2;
	pMesh->getIndexArray()[59] = 3;
	//top
	if (bDrawTopSide)
	{
		pMesh->getIndexArray()[60] = 2;
		pMesh->getIndexArray()[63] = 6;
		pMesh->getIndexArray()[66] = 7;
		pMesh->getIndexArray()[69] = 3;
		pMesh->getIndexArray()[61] = 4;//normals
		pMesh->getIndexArray()[64] = 4;
		pMesh->getIndexArray()[67] = 4;
		pMesh->getIndexArray()[70] = 4;
		pMesh->getIndexArray()[62] = 0;//tex.coords
		pMesh->getIndexArray()[65] = 1;
		pMesh->getIndexArray()[68] = 2;
		pMesh->getIndexArray()[71] = 3;
	}

	//multiply tex coordinates??
	if (pNode->Attribute(XML_TAG_TEX_COORD_MULT_FACTOR))
	{
		double factor = atoi(pNode->Attribute(XML_TAG_TEX_COORD_MULT_FACTOR));
		for (unsigned int i = 0; i < pMesh->getNumTexCoords(); ++i)
			pMesh->getTexCoord(i) *= factor;
		SimpleTLMaterial* pMaterial= dynamic_cast<SimpleTLMaterial*>(m_pMaterialLoaded);
		if (pMaterial)
			pMaterial->setTextureWrapMode(GL_REPEAT);
	}

	pMesh->setNumIndicesPerVertex(3);
	pMesh->setPosOffset(0);
	pMesh->setNormalOffset(1);
	pMesh->setTexCoordOffset(2);
	pMesh->setMaterial(m_pMaterialLoaded);
	pMesh->setPrimitiveType(GL_QUADS);
	addMesh(pMesh);

	m_bb = BoundingBox3D(m_transform.scale()*m_min, m_transform.scale()*m_max);
}

Cilinder::Cilinder(tinyxml2::XMLElement* pNode) : BasicShape(pNode)
{

}

PolyLine::PolyLine(tinyxml2::XMLElement* pNode): BasicShape(pNode)
{
	tinyxml2::XMLElement* pChild;

	//check enough points have been defined
	unsigned int numPoints = countChildren(pNode, XML_TAG_POINT);
	if (numPoints < 2) return;
	
	//material
	int width= 1;
	pChild = pNode->FirstChildElement(XML_TAG_WIDTH);
	if (pChild)
		width = atoi(pChild->GetText());
	Color color= Color(0.0,0.0,0.0,1.0);
	pChild = pNode->FirstChildElement(XML_TAG_COLOR);
	if (pChild)
		XML::load(pChild,color);
	LineMaterial* pMaterial = new LineMaterial();
	pMaterial->setColor(color);
	pMaterial->setWidth(width);

	//read points and construct the mesh
	//points

	Mesh* pMesh = new Mesh();
	pMesh->allocPositions(numPoints);

	pChild = pNode->FirstChildElement(XML_TAG_POINT);
	unsigned int i = 0;
	while (pChild && i<numPoints)
	{
		XML::load(pChild, pMesh->getPosition(i));
		m_bb.addPoint(pMesh->getPosition(i));
		pChild = pChild->NextSiblingElement(XML_TAG_POINT);
		++i;
	}

	pMesh->allocIndices(numPoints);
	for (unsigned int i = 0; i < numPoints; ++i)
		pMesh->getIndexArray()[i] = i;
	pMesh->setPrimitiveType(GL_LINE_STRIP);
	pMesh->setMaterial(pMaterial);
	addMesh(pMesh);
}



