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

#include "FileUtils.h"
#include <algorithm>

#ifdef __unix__
#include <sys/stat.h>
#endif

//returns the last position where a bar character is found in s: '/' or '\\'
//0 if none is found. This is a shortcut because in our context, we can assume that the
//last bar will never be in the 0th position and avoids using std::string::npos outside
size_t getLastBarPos(const string& s)
{
	size_t lastBar = s.find_last_of('/');
	size_t lastBar2 = s.find_last_of('\\');
	if (lastBar == std::string::npos
		|| (lastBar2 != std::string::npos && lastBar2>lastBar) )
		lastBar = lastBar2;

	if (lastBar == std::string::npos)
		return 0;
	return lastBar;
}

string getDirectory(const string& filename)
{
	size_t lastBar = getLastBarPos(filename);
	if (lastBar == 0)
		return "";
	return filename.substr(0, lastBar + 1);
}

string getFilename(const string& filename)
{
	size_t lastBar = getLastBarPos(filename);
	if (lastBar == std::string::npos)
		return "";
	return filename.substr(lastBar + 1);
}

string removeExtension(const string& filename, unsigned int numExtensions)
{
	string result = filename;
	size_t lastExt = filename.find_last_of('.');
	size_t lastBar = getLastBarPos(filename);
	unsigned int numRemovedExtensions = 0;
	while (lastExt > 0 && numRemovedExtensions < numExtensions && lastExt>lastBar)
	{
		result = result.substr(0, lastExt);
		++numRemovedExtensions;

		lastExt = result.find_last_of('.');
	}
	if (numRemovedExtensions == numExtensions)
		return result;
	return filename;
}

bool bFileExists(const string& filename)
{
	struct stat buffer;
	return (stat(filename.c_str(), &buffer) == 0);
}

#if defined(_WIN32) || defined(_WIN64)
#include <direct.h>
#else
#include <unistd.h>
#endif

bool changeWorkingDirectory(const string& directory)
{
#if defined(_WIN32) || defined(_WIN64)
	return _chdir(directory.c_str()) == 0;
#else
	return chdir(directory.c_str()) == 0;
#endif
}