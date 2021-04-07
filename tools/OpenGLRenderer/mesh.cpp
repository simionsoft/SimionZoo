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
#include "mesh.h"
#include "collada-model.h"
#include "xml-load.h"
#include "material.h"
#include <algorithm>

Mesh::Mesh()
{
	m_pPositions = 0;
	m_pNormals = 0;
	m_pTexCoords = 0;
	m_pMaterial = 0;
}

Mesh::~Mesh()
{
	if (m_pPositions != nullptr)
		delete[] m_pPositions;
	if (m_pNormals != nullptr)
		delete[] m_pNormals;
	if (m_pTexCoords != nullptr)
		delete[] m_pTexCoords;
	if (m_pMaterial != nullptr)
		delete m_pMaterial;
}


void Mesh::draw()
{
	if (m_pMaterial)
		m_pMaterial->set();

	glBegin(m_primitiveType);
	for (unsigned int i = 0; i < getNumIndices(); i += m_numIndicesPerVertex)
	{
		if (i + m_posOffset < getNumIndices())
		{
			if (m_pNormals && i + m_normalOffset < getNumIndices())
			{
				int normalIndex = m_pIndices[i + m_normalOffset];
				glNormal3d(m_pNormals[normalIndex].x(), m_pNormals[normalIndex].y(), m_pNormals[normalIndex].z());
			}
			if (m_pTexCoords && i + m_texCoordOffset < getNumIndices())
			{
				int texCoordIndex = m_pIndices[i + m_texCoordOffset];
				glTexCoord2d(m_pTexCoords[texCoordIndex].s(), m_pTexCoords[texCoordIndex].t());
			}
			int posIndex = m_pIndices[i + m_posOffset];
			glVertex3d(m_pPositions[posIndex].x(), m_pPositions[posIndex].y(), m_pPositions[posIndex].z());
		}
	}
	glEnd();
}

void Mesh::updateBoundingBox(BoundingBox3D& bb)
{
	for (unsigned int i = 0; i < m_numPositions; ++i)
		bb.addPoint(m_pPositions[i]);
}

void Mesh::transformVertices(Vector3D& translation, Vector3D& scale)
{
	// transform vertices
	for (unsigned int i= 0; i<m_numPositions; ++i)
	{
		m_pPositions[i] += translation;
		m_pPositions[i] *= scale;
	}
}

void Mesh::flipYZAxis()
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

void Mesh::flipVTexCoord()
{
	for (unsigned int i = 0; i < m_numTexCoords; ++i)
		m_pTexCoords[i].setY(1.0 - m_pTexCoords[i].t());
}

void Mesh::reorderIndices()
{
	int tmp;
	for (unsigned int i = 0; i < getNumIndices() /(3*m_numIndicesPerVertex); i+=3*m_numIndicesPerVertex)
	{
		tmp = m_pIndices[i*m_numIndicesPerVertex + m_posOffset];
		m_pIndices[i*m_numIndicesPerVertex + m_posOffset] = m_pIndices[(i+2)*m_numIndicesPerVertex + m_posOffset];
		m_pIndices[(i + 2)*m_numIndicesPerVertex + m_posOffset] = tmp;
	}
}

void Mesh::ignoreTexCoordsAndNormals()
{
	if (m_pTexCoords)
	{
		delete[] m_pTexCoords;
		m_pTexCoords = nullptr;
	}
	if (m_pNormals)
	{
		delete[] m_pNormals;
		m_pNormals = nullptr;
	}
	m_numIndicesPerVertex = 1;
}