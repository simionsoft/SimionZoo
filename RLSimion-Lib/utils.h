#pragma once
#include <string>
#include <vector>
using namespace std;

class CConfigNode;


class CTable
{
	vector<double> m_columns;
	vector<double> m_rows;
	vector<double> m_values;
	bool m_bSuccess = false;
public:
	CTable();
	~CTable();

	double getMinCol() const;
	double getMaxCol() const;
	double getMinRow() const;
	double getMaxRow() const;
	double getNumCols() const;
	double getNumRows() const;
	double getValue(size_t col, size_t row) const;
	bool readFromFile(string filename);
	double getInterpolatedValue(double colValue, double rowValue) const;
};