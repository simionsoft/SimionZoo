#include "stdafx.h"
#include "utils.h"
#include "globals.h"
#include "parameters.h"

CLASS_CONSTRUCTOR(CDirFileOutput)
{
	DIR_PATH_VALUE(m_outputDir, "Output-Dir", "../data", "The output directory");
	CONST_STRING_VALUE(m_filePrefix, "File-Prefix", "", "The prefix given to all the output files");
	END_CLASS();
}

const char* CDirFileOutput::getOutputDir()
{
	return m_outputDir;
}

const char* CDirFileOutput::getFilePrefix()
{
	return m_filePrefix;
}