// SimionLogViewer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "SimionLogViewer.h"
#include "../OpenGLRenderer/timer.h"

#include <algorithm>
using namespace std;



int main(int argc, char** argv)
{
	SimionLogViewer viewer;

	if (argc <= 1) return -1;

	viewer.init(argc, argv);

	if (viewer.loadLogFile(argv[1]))
	{
		while (!viewer.bExitRequested())
		{
			viewer.handleInput();

			viewer.draw();
		}
	}
    return 0;
}

