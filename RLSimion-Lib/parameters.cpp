#include "stdafx.h"
#include "parameters.h"


template<class DataType> DataType& CLASS_PARAM<DataType>::get()
{
	return m_value;
}