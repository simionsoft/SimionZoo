#pragma once

#include <string.h>
#include <stdio.h>
#include <stddef.h>
using namespace std;

namespace CrossPlatform
{


	template<typename ...Args>
	int sprintf_s(char* buffer, int bufferSize, const char* format, Args... args)
	{
#ifdef __WIN32
		return sprintf_s(buffer, bufferSize, format, args...);
#else
		return sprintf(buffer, format, args...);
#endif
	}
	void fopen_s(FILE** pOutFile, const char* filename, const char* mode)
	{
#ifdef __WIN32
		fopen_s(&m_functionLogFile, m_outputFunctionLogBinary.c_str(), "wb");
#else
		*pOutFile = fopen(filename, mode);
#endif
	}
	size_t fread_s(void *buffer, size_t bufferSize, size_t elementSize, size_t count, FILE *stream)
	{
#ifdef __WIN32
		return fread_s(buffer, bufferSize, elementSize, count, stream);
#else
		return fread(buffer, elementSize, count, stream);
#endif
	}

	char* strcpy_s(char* dst, size_t dstSize, const char *src)
	{
#ifdef __WIN32
		return strcpy_s(dst, dstSize, src);
#else
		return strcpy(dst, src);
#endif
	}

	void strcat_s(char* dst, size_t dstSize, const char* src)
	{
#ifdef __WIN32
		strcat_s(dst, dstSize, src);
#else
		strcat(dst, src);
#endif
	}
	void memcpy_s(void* dst, size_t dstSize, const void* src, size_t numBytes)
	{
#ifdef __WIN32
		memcpy_s(dst, dstSize, src, numBytes);
#else
		memcpy(dst, src, numBytes);
#endif
	}
}