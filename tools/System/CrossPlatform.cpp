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

#include "CrossPlatform.h"
#include <algorithm>

namespace CrossPlatform
{
	void ForceUseFolderCharacter(string& commandLine)
	{
#ifdef _WIN32
		replace(commandLine.begin(), commandLine.end(), '/', '\\');
#else
		replace(commandLine.begin(), commandLine.end(), '\\', '/');
#endif

	}

	void Fopen_s(FILE** pOutFile, const char* filename, const char* mode)
	{
#ifdef _WIN32
		fopen_s(pOutFile, filename, mode);
#else
		*pOutFile = fopen(filename, mode);
#endif
	}

	size_t Fread_s(void *buffer, size_t bufferSize, size_t elementSize, size_t count, FILE *stream)
	{
#ifdef _WIN32
		return fread_s(buffer, bufferSize, elementSize, count, stream);
#else
		return fread(buffer, elementSize, count, stream);
#endif
	}



	char* Strcpy_s(char* dst, size_t dstSize, const char *src)
	{
#ifdef _WIN32
		strcpy_s(dst, dstSize, src);
		return dst;
#else
		return strcpy(dst, src);
#endif
	}

	void Strcat_s(char* dst, size_t dstSize, const char* src)
	{
#ifdef _WIN32
		strcat_s(dst, dstSize, src);
#else
		strcat(dst, src);
#endif
	}

	void Memcpy_s(void* dst, size_t dstSize, const void* src, size_t numBytes)
	{
#ifdef _WIN32
		memcpy_s(dst, dstSize, src, numBytes);
#else
		memcpy(dst, src, numBytes);
#endif
	}

}