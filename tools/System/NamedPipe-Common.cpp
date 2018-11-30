#include "NamedPipe.h"

#include <iostream>

void NamedPipe::logMessage(const char* message)
{
	if (m_bVerbose)
	{
		std::cout << message << "\n";
	}
}