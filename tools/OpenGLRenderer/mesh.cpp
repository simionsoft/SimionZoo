#include "stdafx.h"
#include "mesh.h"
#include "collada-model.h"
#include "xml-load.h"
#include "material.h"
#include <algorithm>

CMesh::CMesh()
{
	m_pPositions = 0;
	m_pNormals = 0;
	m_pTexCoords = 0;
	m_pIndices = 0;
	m_pMaterial = 0;
}

CMesh::~CMesh()
{
	if (m_pPositions != nullptr)
		delete[] m_pPositions;
	if (m_pNormals != nullptr)
		delete[] m_pNormals;
	if (m_pTexCoords != nullptr)
		delete[] m_pTexCoords;
	if (m_pIndices != nullptr)
		delete[] m_pIndices;
	if (m_pMaterial != nullptr)
		delete m_pMaterial;
}


void CMesh::draw()
{
	if (m_pMaterial)
		m_pMaterial->set();

	glBegin(m_primitiveType);

	for (unsigned int i = 0; i<m_numIndices; i+=m_numIndicesPerVertex)
	{
		if (m_pNormals)
		{
			int normalIndex = m_pIndices[i + m_normalOffset];
			glNormal3d(m_pNormals[normalIndex].x(), m_pNormals[normalIndex].y(), m_pNormals[normalIndex].z());
		}
		if (m_pTexCoords)
		{
			int texCoordIndex = m_pIndices[i + m_texCoordOffset];
			glTexCoord2d(m_pTexCoords[texCoordIndex].s(), m_pTexCoords[texCoordIndex].t());
		}
		int posIndex = m_pIndices[i + m_posOffset];
		glVertex3d(m_pPositions[posIndex].x(), m_pPositions[posIndex].y(), m_pPositions[posIndex].z());
	}
	glEnd();
}

void CMesh::updateBoundingBox(BoundingBox3D& bb)
{
	for (unsigned int i = 0; i < m_numPositions; ++i)
		bb.addPoint(m_pPositions[i]);
}

void CMesh::transformVertices(Vector3D& translation, Vector3D& scale)
{
	// transform vertices
	for (unsigned int i= 0; i<m_numPositions; ++i)
	{
		m_pPositions[i] += translation;
		m_pPositions[i] *= scale;
	}
}

void CMesh::flipYZAxis()
{
	double tmp;
	// transform vertices
	for (unsigned int i = 0; i<m_numPositions; ++i)
	{
		tmp = m_pPositions[i].z();
		m_pPositions[i].setZ(-m_pPositions[i].y());
		m_pPositions[i].setY(tmp);
	}
	//transform normals
	for (unsigned int i = 0; i < m_numNormals; ++i)
	{
		tmp = m_pNormals[i].z();
		m_pNormals[i].setZ(-m_pNormals[i].y());
		m_pNormals[i].setY(tmp);
	}
}

void CMesh::flipVTexCoord()
{
	for (unsigned int i = 0; i < m_numTexCoords; ++i)
		m_pTexCoords[i].setY(1.0 - m_pTexCoords[i].t());
}

void CMesh::reorderIndices()
{
	int tmp;
	for (unsigned int i = 0; i < m_numIndices/(3*m_numIndicesPerVertex); i+=3*m_numIndicesPerVertex)
	{
		tmp = m_pIndices[i*m_numIndicesPerVertex + m_posOffset];
		m_pIndices[i*m_numIndicesPerVertex + m_posOffset] = m_pIndices[(i+2)*m_numIndicesPerVertex + m_posOffset];
		m_pIndices[(i + 2)*m_numIndicesPerVertex + m_posOffset] = tmp;
	}
}