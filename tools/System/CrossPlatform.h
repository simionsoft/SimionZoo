#pragma once

#include <string.h>
#include <stdio.h>
#include <stddef.h>
using namespace std;

namespace CrossPlatform
{
	template<typename ...Args>
	int Sprintf_s(char* buffer, int bufferSize, const char* format, Args... args)
	{
#ifdef _WIN32
		return sprintf_s(buffer, bufferSize, format, args...);
#else
		return sprintf(buffer, format, args...);
#endif
	}

	template<typename ...Args>
	int Sscanf_s(const char *buffer, const char *format, Args... args)
	{
#ifdef _WIN32
		return sscanf_s(buffer, format, args...);
#else
		return sscanf(buffer, format, args...);
#endif
	}

	template<typename ...Args>
	int Fprintf_s(FILE *stream, const char *format, Args... args)
	{
#ifdef _WIN32
		return fprintf_s(stream, format, args...);
#else
		return fprintf(stream, format, args...);
#endif
	}

	void Fopen_s(FILE** pOutFile, const char* filename, const char* mode);

	size_t Fread_s(void *buffer, size_t bufferSize, size_t elementSize, size_t count, FILE *stream);

	char* Strcpy_s(char* dst, size_t dstSize, const char *src);

	void Strcat_s(char* dst, size_t dstSize, const char* src);

	void Memcpy_s(void* dst, size_t dstSize, const void* src, size_t numBytes);
}