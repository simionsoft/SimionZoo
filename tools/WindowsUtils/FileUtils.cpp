#include "FileUtils.h"

string getDirectory(const string& pFilename)
{
	int last = pFilename.find_last_of('/', '\\');
	if (last < 0)
		return string("");
	return string(pFilename.substr(0, last + 1));
}

bool bFileExists(const string& filename)
{
	struct stat buffer;
	return (stat(filename.c_str(), &buffer) == 0);
}