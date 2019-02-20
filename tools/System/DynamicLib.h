#pragma once

class DynamicLib
{
	const char* m_libPath;
	void* m_handle;

public:
	
	DynamicLib();
	virtual ~DynamicLib();

	//libPath: must be a relative path to the library beginning with "./"
	//If called more than once, subsequent calls will be ignored until Unload() is called
	void Load(const char* libPath);
	void Unload();
	void* GetFuncAddress(const char* funcName);
	bool IsLoaded();
};

