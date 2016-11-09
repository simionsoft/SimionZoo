#pragma once


template<class DataType> class CParameter
{
	DataType value;
public:
	virtual DataType& get();
};