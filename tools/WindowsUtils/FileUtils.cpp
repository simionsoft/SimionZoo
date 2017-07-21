#include "FileUtils.h"

string getDirectory(const string& filename)
{
	size_t last = filename.find_last_of('/', '\\');
	if (last < 0)
		return "";
	return filename.substr(0, last + 1);
}

string getFilename(const string& filepath)
{
	size_t last = filepath.find_last_of('/', '\\');
	if (last < 0)
		return "";
	return filepath.substr(last + 1);
}

string removeExtension(const string& filename, unsigned int numExtensions)
{
	string result = filename;
	size_t lastExt, lastBar;
	lastBar = filename.find_last_of('/', '\\');
	lastExt = result.find_last_of('.');
	int numRemovedExtensions = 0;
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