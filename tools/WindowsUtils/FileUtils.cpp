#include "FileUtils.h"

string getDirectory(string pFilename)
{
	int last = pFilename.find_last_of('/', '\\');
	if (last < 0)
		return string("");
	return string(pFilename.substr(0, last + 1));
}