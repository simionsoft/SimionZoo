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
			process1.spawn("C://Windows//System32//cmd.exe");
#else
			process1.spawn("/bin/ping 127.0.0.1 -c5");
#endif
			this_thread::sleep_for(chrono::milliseconds(200));
			Assert::IsTrue(process1.isRunning());

			process1.stop();

			this_thread::sleep_for(chrono::milliseconds(200));
			Assert::IsFalse(process1.isRunning());
		}
		TEST_METHOD(Process_Run_Wait)
		{
			Process process1;
#ifdef _WIN32
			process1.spawn("C://Windows//System32//ping.exe 127.0.0.1 -n 2");
#else
			process1.spawn("/bin/ping 127.0.0.1 -c2");
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

		TEST_METHOD(CrossPlatform_Split)
		{
			vector<string> splitParts = CrossPlatform::split("oh my god what is this?", ' ');
			Assert::AreEqual((size_t) 6, splitParts.size());
			Assert::AreEqual("oh", splitParts[0].c_str());
			Assert::AreEqual("my", splitParts[1].c_str());
			Assert::AreEqual("god", splitParts[2].c_str());
			Assert::AreEqual("what", splitParts[3].c_str());
			Assert::AreEqual("is", splitParts[4].c_str());
			Assert::AreEqual("this?", splitParts[5].c_str());

			splitParts = CrossPlatform::split("oh my god what is this?", ',');
			Assert::AreEqual((size_t)1, splitParts.size());

			splitParts = CrossPlatform::split("", ' ');
			Assert::AreEqual((size_t)0, splitParts.size());

			splitParts = CrossPlatform::split("a::b", ':');
			Assert::AreEqual((size_t)3, splitParts.size());
			Assert::AreEqual("a", splitParts[0].c_str());
			Assert::AreEqual("", splitParts[1].c_str());
			Assert::AreEqual("b", splitParts[2].c_str());
		}
	};
}
