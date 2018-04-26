#include "stdafx.h"
#include "material.h"
#include <gl\GL.h>
#include <algorithm>

//char* m_pTexelBuffer = nullptr;
//unsigned int m_sizeX = 0, m_sizeY = 0;

UnlitLiveTextureMaterial::UnlitLiveTextureMaterial(unsigned int sizeX, unsigned int sizeY)
{
	m_sizeX = sizeX;
	m_sizeY = sizeY;
	m_numTexels = sizeX * sizeY;
	m_numChannels = 4;
	m_pTexelBuffer = new float[m_numTexels * m_numChannels];

	//initialize it as green
	for (int i = 0; i < m_numTexels; ++i)
	{
		m_pTexelBuffer[i*m_numChannels + 0] = 0.0f;
		m_pTexelBuffer[i*m_numChannels + 1] = 0.0;
		m_pTexelBuffer[i*m_numChannels + 2] = 0.0f;
		m_pTexelBuffer[i*m_numChannels + 3] = 0.0f;
	}
	glGenTextures(1, &m_textureId);
}

UnlitLiveTextureMaterial::~UnlitLiveTextureMaterial()
{
	delete [] m_pTexelBuffer;
}

void UnlitLiveTextureMaterial::updateTexture(double* pBuffer)
{
	//calculate min and max
	double value;
	double minValue = std::numeric_limits<double>::max(), maxValue= std::numeric_limits<double>::min();
	for (unsigned int i = 0; i < m_numTexels; i++)
	{
		value = pBuffer[i];
		if (value < minValue) minValue = value;
		if (value > maxValue) maxValue = value;
	}
	//map double values to colors
	Color maxValueColor = Color(1.0, 0.0, 0.0);
	Color minValueColor = Color(0.0, 0.0, 1.0);
	Color texelColor;
	double normValue;
	double valueRange = std::max(0.01, maxValue - minValue);
	for (unsigned int i = 0; i < m_numTexels; i++)
	{
		normValue = (pBuffer[i] - minValue) / valueRange;
		texelColor = minValueColor.lerp(maxValueColor,normValue);
		m_pTexelBuffer[i*m_numChannels + 0] = texelColor.r();
		m_pTexelBuffer[i*m_numChannels + 1] = texelColor.g();
		m_pTexelBuffer[i*m_numChannels + 2] = texelColor.b();
		m_pTexelBuffer[i*m_numChannels + 3] = texelColor.a();
	}
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_sizeX, m_sizeY, 0, GL_RGBA, GL_FLOAT, m_pTexelBuffer);

	GLenum error = glGetError();
}

void UnlitLiveTextureMaterial::set()
{
	GLenum error;
	error = glGetError();
	//no alpha blending
	glDisable(GL_LIGHTING);
	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
	error = glGetError();

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_textureId);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	error = glGetError();
}