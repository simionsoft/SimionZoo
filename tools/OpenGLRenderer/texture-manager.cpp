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
#include "texture-manager.h"
#include "renderer.h"


Texture::Texture()
{
	numRefs = 1;
}

Texture::~Texture()
{
	Renderer::get()->logMessage(string("Deleting texture object: ") + to_string(oglId) + string("(")
		+ to_string(numRefs) + string(" references)"));
	glDeleteTextures(1, &oglId);
}

TextureManager::TextureManager()
{
}


TextureManager::~TextureManager()
{
	for (auto it = m_textures.begin(); it != m_textures.end(); ++it)
	{
		delete *it;
	}
	Renderer::get()->logMessage("Finished freeing texture objects");
}

size_t TextureManager::loadTexture(string filename)
{
	filename = m_folder + filename;

	Renderer::get()->logMessage(string("Loading texture file: ") + filename);
	int id = 0;
	for (auto it = m_textures.begin(); it != m_textures.end(); ++it)
	{
		if ((*it)->path == filename)
		{
			(*it)->addRef();
			Renderer::get()->logMessage("Already loaded");
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
		Renderer::get()->logMessage("Success");
		Texture* pTexture = new Texture();
		pTexture->oglId = oglId;
		pTexture->path = filename;
		m_textures.push_back(pTexture);

		return m_textures.size() - 1; //we return the internal ID instead of the OpenGL id
	}
	Renderer::get()->logMessage("Failed: could not find file");
	return -1;
}

void TextureManager::set(int textureId)
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

void Texture::set()
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, oglId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
}