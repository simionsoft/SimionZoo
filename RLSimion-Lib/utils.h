#pragma once
#include <string>
#include <vector>
using namespace std;

class CConfigNode;

class CFilePathList
{
	char** m_pBuffer;
	int m_numFilePaths;
public:
	static const int m_numMaxFilePaths = 1024;
	static const int m_filePathMaxSize = 1024;

	CFilePathList();
	virtual ~CFilePathList();

	void clear();

	void addFilePath(const char* filepath);
	int getNumFilePaths() { return m_numFilePaths; }
	const char* getFilePath(int i);
};

class CTable
{
	vector<double> m_columns;
	vector<double> m_rows;
	vector<double> m_values;
	bool m_bSuccess = false;
public:
	CTable();
	~CTable();

	double getValue(int col, int row);
	bool readFromFile(string filename);
	double getInterpolatedValue(double colValue, double rowValue);
};