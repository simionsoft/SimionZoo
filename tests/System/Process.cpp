#include "CppUnitTest.h"

#include "../../tools/System/Process.h"
#include "../../tools/System/NamedPipe.h"
#include "../../tools/System/CrossPlatform.h"
#include <thread>
#include <chrono>
using namespace std;

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#define BUFFER_SIZE 256
#define MESSAGE "Hello my dear!"
char serverBuffer[BUFFER_SIZE];
NamedPipeClient clientPipe;
NamedPipeServer serverPipe;

namespace SystemTests
{		
	TEST_CLASS(System_Tests)
	{
	public:
		
		TEST_METHOD(Process_Run_Stop)
		{
			Process process1;
#ifdef _WIN32
			process1.spawn("C://Windows//System32//cmd.exe", false);
#else
			process1.spawn("/bin/ls -R /", false);
#endif

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
#ifdef _WIN32
			process1.spawn("C://Windows//System32//ping.exe 127.0.0.1 -n 2", false);
#else
			process1.spawn("/bin/ping 127.0.0.1 -c 2", false);
#endif
			Assert::IsTrue(process1.isRunning());

			process1.wait();

			this_thread::sleep_for(chrono::milliseconds(400));
			Assert::IsFalse(process1.isRunning());
		}


		
		static void serverThread()
		{
			serverPipe.openNamedPipeServer("test");
			bool bConnected = serverPipe.waitForClientConnection();
			if (!bConnected) return;
			
			serverPipe.readToBuffer(serverBuffer, BUFFER_SIZE);
			serverPipe.closeServer();
		}

		static void clientThread()
		{

			bool bConnected= clientPipe.connectToServer("test");
			if (!bConnected) return;

			char clientBuffer[BUFFER_SIZE];
			CrossPlatform::Strcpy_s(clientBuffer, BUFFER_SIZE, MESSAGE);
			clientPipe.writeBuffer(clientBuffer, strlen(clientBuffer));
			clientPipe.closeConnection();
		}

		TEST_METHOD(NamedPipes)
		{
			std::thread server(serverThread);
			std::thread client(clientThread);

			server.join();
			client.join();
			Assert::AreEqual(MESSAGE, serverBuffer);
			Assert::IsTrue(!serverPipe.isConnected());
			Assert::IsTrue(!clientPipe.isConnected());
		}

		TEST_METHOD(CrossPlatform_CStrings)
		{
			char buffer[BUFFER_SIZE];
			CrossPlatform::Sprintf_s(buffer, BUFFER_SIZE, "One=%d and Two=%d", 1, 2);
			Assert::AreEqual("One=1 and Two=2", buffer);

			string command = "cd /home/bortx/";
			CrossPlatform::ForceUseFolderCharacter(command);
#ifdef _WIN32
			Assert::AreEqual("cd \\home\\bortx\\", command.c_str());
#else
			Assert::AreEqual("cd /home/bortx/", command.c_str());
#endif

			CrossPlatform::Strcpy_s(buffer, BUFFER_SIZE, "Hello my darling!");
			Assert::AreEqual("Hello my darling!", buffer);
		}

		TEST_METHOD(CrossPlatform_Memory)
		{
			char buffer[BUFFER_SIZE];
			char dstBuffer[BUFFER_SIZE];
			dstBuffer[2] = 27;
			buffer[0] = 0; buffer[1] = 1;
			CrossPlatform::Memcpy_s(dstBuffer, BUFFER_SIZE, buffer, 2);
			Assert::AreEqual((char)0, dstBuffer[0]);
			Assert::AreEqual((char)1, dstBuffer[1]);
			Assert::AreEqual((char)27, dstBuffer[2]);
		}
	};
}