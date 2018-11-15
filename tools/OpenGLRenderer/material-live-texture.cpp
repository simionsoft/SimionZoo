#include "stdafx.h"
#include "material.h"
#include "../../3rd-party/glew2/include/glew.h"
#include <algorithm>
#include <stdexcept>

UnlitLiveTextureMaterial::UnlitLiveTextureMaterial(unsigned int sizeX, unsigned int sizeY)
{
	m_sizeX = sizeX;
	m_sizeY = sizeY;
	m_numTexels = sizeX * sizeY;
	m_numChannels = 4;
	m_pTexelBuffer = new float[m_numTexels * m_numChannels];

	//initialize it as blue
	for (size_t i = 0; i < m_numTexels; ++i)
	{
		m_pTexelBuffer[i*m_numChannels + 0] = 0.f;
		m_pTexelBuffer[i*m_numChannels + 1] = 0.f;
		m_pTexelBuffer[i*m_numChannels + 2] = 1.f;
		m_pTexelBuffer[i*m_numChannels + 3] = 0.f;
	}
	glGenTextures(1, &m_textureId);
	glBindTexture(GL_TEXTURE_2D, m_textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_sizeX, m_sizeY, 0, GL_RGBA, GL_FLOAT, m_pTexelBuffer);
}

UnlitLiveTextureMaterial::~UnlitLiveTextureMaterial()
{
	delete [] m_pTexelBuffer;
}

void UnlitLiveTextureMaterial::updateTexture(const vector<double>& pBuffer)
{
	if (m_numTexels != pBuffer.size())
		throw std::runtime_error("Buffer with missmatched size provided to UnlitLiveTextureMaterial::updateTexture()");

	//calculate min and max
	double value;
	double minValue = std::numeric_limits<double>::max(), maxValue= std::numeric_limits<double>::lowest();
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
	double valueRange = maxValue - minValue;
	for (unsigned int i = 0; i < m_numTexels; i++)
	{
		if (valueRange!=0.0)
			normValue = (pBuffer[i] - minValue) / valueRange;
		else normValue = 0.0;

		texelColor = minValueColor.lerp(maxValueColor,normValue);
		m_pTexelBuffer[i*m_numChannels + 0] = texelColor.r();
		m_pTexelBuffer[i*m_numChannels + 1] = texelColor.g();
		m_pTexelBuffer[i*m_numChannels + 2] = texelColor.b();
		m_pTexelBuffer[i*m_numChannels + 3] = texelColor.a();
	}
	glBindTexture(GL_TEXTURE_2D, m_textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_sizeX, m_sizeY, 0, GL_RGBA, GL_FLOAT, m_pTexelBuffer);
}

void UnlitLiveTextureMaterial::set()
{
	//no alpha blending
	glDisable(GL_LIGHTING);
	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_textureId);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}