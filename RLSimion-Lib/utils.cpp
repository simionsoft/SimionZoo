#include "stdafx.h"
#include "utils.h"
#include "globals.h"
#include "parameters.h"

CLASS_CONSTRUCTOR(CString)
{
	CONST_STRING_VALUE(m_string, "Value", "", "The value of the string");
	END_CLASS();
}