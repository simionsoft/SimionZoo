#include "DynamicLib.h"
#include <stdio.h>
#include <dlfcn.h>

DynamicLib::DynamicLib()
{
	m_handle = 0;
	m_libPath = 0;
}

DynamicLib::~DynamicLib()
{
	Unload();
}

bool DynamicLib::IsLoaded()
{
	return m_handle != 0;
}


void DynamicLib::Load(const char* libPath)
{
	if (!IsLoaded())
	{
		m_libPath = libPath;
		m_handle = dlopen(m_libPath, RTLD_LAZY | RTLD_GLOBAL);
		if (!m_handle)
		{
			fprintf(stderr, "Error: %s\n", dlerror());
		}

	}
}

void DynamicLib::Unload()
{
	if (IsLoaded())
	{
		m_libPath = nullptr;
		dlclose(m_handle);
		m_handle = 0;
	}
}

void* DynamicLib::GetFuncAddress(const char* funcName)
{
	return dlsym(m_handle, funcName);
}
