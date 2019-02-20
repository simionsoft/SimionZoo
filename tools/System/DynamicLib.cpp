#include "DynamicLib.h"

#define WINDOWS_MEAN_AND_LEAN
#include <windows.h>
#undef min
#undef max

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
		m_handle = (void*) LoadLibrary(m_libPath);
	}
}

void DynamicLib::Unload()
{
	if (IsLoaded())
	{
		m_libPath = nullptr;
		FreeLibrary((HMODULE) m_handle);
		m_handle = 0;
	}
}

void* DynamicLib::GetFuncAddress(const char* funcName)
{
	return GetProcAddress((HMODULE) m_handle, funcName);
}
