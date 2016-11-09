#include "stdafx.h"
#include "parameters.h"


template<class DataType> DataType& CParameter<DataType>::get()
{
	return value;
}