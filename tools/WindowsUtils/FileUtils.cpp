#include "FileUtils.h"
#include <algorithm>

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