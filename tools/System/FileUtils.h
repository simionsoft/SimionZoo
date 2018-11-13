#pragma once

#include <string>
using namespace std;

size_t getLastBarPos(const string& s);
string getDirectory(const string& filename);
string removeExtension(const string& filename, unsigned int numExtensions = 1);
string getFilename(const string& filepath);
bool bFileExists(const string& filename);
