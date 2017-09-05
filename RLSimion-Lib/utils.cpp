#include "stdafx.h"
#include "utils.h"
#include "config.h"
#include <algorithm>
#include <fstream>

CFilePathList::CFilePathList()
{
	m_pBuffer = new char*[m_numMaxFilePaths];
	m_numFilePaths = 0;
}

CFilePathList::~CFilePathList()
{
	for (int i = 0; i < m_numFilePaths; i++)
		delete [] m_pBuffer[i];
	delete[] m_pBuffer;
}

void CFilePathList::clear()
{
	for (int i = 0; i < m_numFilePaths; i++)
		delete[] m_pBuffer[i];
	m_numFilePaths = 0;
}

void CFilePathList::addFilePath(const char* filepath)
{
	if (m_numFilePaths < m_numMaxFilePaths - 1)
	{
		m_pBuffer[m_numFilePaths] = new char[m_filePathMaxSize];
		strcpy_s(m_pBuffer[m_numFilePaths], m_filePathMaxSize, filepath);
		m_numFilePaths++;
	}
}

const char* CFilePathList::getFilePath(int i)
{
	if (i < m_numFilePaths) return m_pBuffer[i];

	return 0;
}



CTable::CTable()
{
}

CTable::~CTable()
{
}

unsigned int parseHeader(const char* pInputString, vector<double>& outVector, char delimChar= '\t')
{
	unsigned int numParsedValues = 0;
	if (!pInputString) return 0;

	const char* pt = pInputString;
	if (*pt == delimChar || *pt == '\n') ++pt; //skip initial space/line jump
	while (*pt != 0)
	{
		outVector.push_back (atof(pt));
		++numParsedValues;

		while (*pt != 0 && *pt != delimChar && *pt != '\n') ++pt;
		if (*pt == delimChar || *pt == '\n') ++pt;
	}
	return numParsedValues;
}

bool CTable::readFromFile(string filename)
{
	//Reads a matrix from a file. The format should be:
//		3	5	7	9
//9		1.2	3.2	-3	-3.4
//10	2.3	2.5	-3	-4.2
	char line[1024];
	char* pChar= line;
	ifstream inputFile(filename);
	if (inputFile.is_open())
	{
		//read the column headers
		inputFile.getline(line,1024);
		int numColumns= parseHeader(line, m_columns, '\t');
		int numRows = 0;
		double value;
		while (!inputFile.eof())
		{
			inputFile >> value;
			m_rows.push_back(value);
			for (int i = 0; i < numColumns; ++i)
			{
				inputFile >> value;
				m_values.push_back(value);
			}
			++numRows;
		}

		inputFile.close();

		if (numRows*numColumns == m_values.size())
		{
			m_bSuccess = true;
			return true;
		}
	}
	m_bSuccess = false;
	return false;
}

double CTable::getInterpolatedValue(double columnValue, double rowValue) const
{
	//check column and row values are within the defined ranges
	columnValue = std::max(m_columns[0], std::min(m_columns[m_columns.size() - 1], columnValue));
	rowValue = std::max(m_rows[0], std::min(m_rows[m_rows.size() - 1], rowValue));

	//search for the columns/rows where the given values are in
	int colIndex = 1, rowIndex = 1;
	while (m_columns[colIndex] < columnValue) ++colIndex;
	while (m_rows[rowIndex] < rowValue) ++rowIndex;
	--colIndex;
	--rowIndex;

	//interpolation factors
	double colU, rowU, colRange, rowRange;
	colRange = m_columns[colIndex + 1] - m_columns[colIndex];
	rowRange = m_rows[rowIndex + 1] - m_rows[rowIndex];
	colU = (columnValue - m_columns[colIndex]) / colRange;
	rowU = (rowValue - m_rows[rowIndex]) / rowRange;
	double value= 0.0;
	value += (1 - colU) * (1- rowU) * getValue(colIndex,rowIndex);
	value += (1 - colU) * rowU * getValue(colIndex,rowIndex + 1);
	value += colU * (1 - rowU) * getValue(colIndex + 1, rowIndex);
	value += colU * rowU * getValue(colIndex + 1, rowIndex + 1);
	return value;
}

double CTable::getValue(size_t col, size_t row) const
{
	col = std::max((size_t)0, std::min(col, m_columns.size() - 1));
	row = std::max((size_t)0, std::min(row, m_rows.size() - 1));

	return m_values[row*m_columns.size() + col];
}

double CTable::getMinCol() const
{
	if (!m_bSuccess) return 0.0;
	return m_columns[0];
}

double CTable::getMaxCol() const
{
	if (!m_bSuccess) return 0.0;
	return m_columns[m_columns.size() - 1];
}

double CTable::getMinRow() const
{
	if (!m_bSuccess) return 0.0;
	return m_rows[0];
}

double CTable::getMaxRow() const
{
	if (!m_bSuccess) return 0.0;
	return m_rows[m_rows.size() - 1];
}

double CTable::getNumCols() const
{
	if (!m_bSuccess) return m_columns.size();
	return 0;
}

double CTable::getNumRows() const
{
	if (!m_bSuccess) return m_rows.size();
	return 0;
}