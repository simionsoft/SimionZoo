#include "stdafx.h"
#include "texture-manager.h"
#include "renderer.h"


CTexture::CTexture()
{}

CTexture::~CTexture()
{
	glDeleteTextures(1, &oglId);
}

CTextureManager::CTextureManager()
{
}


CTextureManager::~CTextureManager()
{
	for (vector<CTexture*>::iterator it = m_textures.begin(); it != m_textures.end(); ++it)
	{
		delete *it;
	}
}

size_t CTextureManager::loadTexture(string filename)
{
	filename = m_folder + filename;
	int id = 0;
	for (vector<CTexture*>::iterator it = m_textures.begin(); it != m_textures.end(); ++it)
	{
		if ((*it)->path == filename)
		{
			(*it)->addRef();
			return id;
		}
		++id;
	}
	//texture not found, must load it
	unsigned int numChannelsRead;
	int oglId = SOIL_load_OGL_texture(filename.c_str(), 0, 0
		, SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS,&numChannelsRead);
	if (oglId != 0)
	{
		CTexture* pTexture = new CTexture();
		pTexture->oglId = oglId;
		pTexture->path = filename;
		m_textures.push_back(pTexture);

		return m_textures.size() - 1; //we return the internal ID instead of the OpenGL id
	}
	return -1;
}

void CTextureManager::set(int textureId)
{
	if (textureId >= 0 && textureId < (int)m_textures.size() )
	{
		m_textures[textureId]->set();
	}
	else
	{
		glDisable(GL_TEXTURE_2D);
	}
}

void CTexture::set()
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, oglId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
}