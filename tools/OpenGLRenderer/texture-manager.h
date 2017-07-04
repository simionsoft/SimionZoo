#pragma once

#include <vector>
#include <string>
using namespace std;

class CTexture
{
	unsigned int numRefs = 0;
public:
	int wrap_s=GL_TEXTURE_WRAP_S, wrap_t= GL_TEXTURE_WRAP_T;
	int minFilter = GL_LINEAR_MIPMAP_LINEAR, magFilter = GL_LINEAR_MIPMAP_LINEAR;
	string path;
	unsigned int oglId= 0;

	void addRef() { ++numRefs; }
	void set();

	CTexture();
	~CTexture();
};

class CTextureManager
{
	string m_folder;
	vector<CTexture*> m_textures;
public:
	CTextureManager();
	virtual ~CTextureManager();

	size_t loadTexture(string filename);
	void set(int textureId);

	void setFolder(string folder) { m_folder = folder; }
	string getFolder() { return m_folder; }
};

