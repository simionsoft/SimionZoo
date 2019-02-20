#include "CppUnitTest.h"

#include "../../../tools/System/Process.h"
#include <thread>
#include <chrono>
using namespace std;

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ProcessTest
{		
	TEST_CLASS(Process_Windows)
	{
	public:
		
		TEST_METHOD(Process_Run_Stop)
		{
			Process process1;
			process1.spawn("C://Windows//System32//cmd.exe", false);

			this_thread::sleep_for(chrono::milliseconds(400));
			Assert::IsTrue(process1.isRunning());

			this_thread::sleep_for(chrono::milliseconds(400));
			process1.stop();

			this_thread::sleep_for(chrono::milliseconds(400));
			Assert::IsFalse(process1.isRunning());
		}
		TEST_METHOD(Process_Run_Wait)
		{
			Process process1;
			process1.spawn("C://Windows//System32//ping.exe 127.0.0.1 -n 1", false);

			Assert::IsTrue(process1.isRunning());

			process1.wait();

			this_thread::sleep_for(chrono::milliseconds(400));
			Assert::IsFalse(process1.isRunning());
		}
	};
}