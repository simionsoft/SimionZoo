#include <stdafx.h>
#include "logger.h"
#include "parameters.h"
#include "world.h"
#include "states-and-actions.h"
#include "globals.h"
#include "experiment.h"








CLogger::~CLogger()
{
}

CLogger::CLogger(char* configFile)
{


	delete pParameters;
}
