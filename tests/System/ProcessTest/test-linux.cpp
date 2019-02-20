#include "../../../tools/System/Process.h"
#include <chrono>
#include <thread>
#include <iostream>
using namespace std;

int main(int argc, char** argv)
{
	Process process1;

	cout << "#### 1st test: Run -> force_to_stop\n\n";
	process1.setVerbose(true);
	process1.spawn("/bin/sleep", false, "10");

	process1.isRunning();

	this_thread::sleep_for(chrono::milliseconds(500));

	process1.stop();

	this_thread::sleep_for(chrono::milliseconds(500));

	process1.isRunning();

	

	cout << "\n\n#### 2nd test: Run -> wait\n\n";

	Process process2;
	process2.setVerbose(true);
	process2.spawn("/bin/sleep", false, "10");

	process2.isRunning();

	this_thread::sleep_for(chrono::milliseconds(500));

	process2.wait();

	process2.isRunning();

	cout << "Parent process finished\n";
	return 0;
}