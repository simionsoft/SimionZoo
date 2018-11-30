using System;
using System.Collections.Generic;
using System.IO.Pipes;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace NamedPipeEchoServer
{
    class Program
    {
        /// <summary>
        /// This program opens a NamedPipeServerStream and waits for messages from clients who connect to it
        /// Used to test NamedPipe-linux.cpp
        /// If given, the 1st argument is the name of the pipe, else the name will be "test-pipe"
        /// </summary>
        /// <param name="args"></param>

        static void Main(string[] args)
        {
            int bufferSize = 1024;
            byte[] buffer = new byte[bufferSize];

            Console.WriteLine("Server starting");

            string pipeName = "test-pipe";

            if (args.Count() > 0)
                pipeName = args[0];
            NamedPipeServerStream pipeServer = new NamedPipeServerStream(pipeName);

            try
            {
                if (pipeServer != null)
                {
                    Console.WriteLine("Waiting for client process to connect to the server's pipe: " + pipeName);

                    pipeServer.WaitForConnection();

                    Console.WriteLine("Client process connected to the Herd Agent's pipe");

                    while (pipeServer.IsConnected)
                    {
                        Console.WriteLine("Waiting for a message from the spawned process");

                        int numBytesRead = pipeServer.Read(buffer, 0, bufferSize);

                        if (numBytesRead>0)
                            Console.WriteLine(System.Text.Encoding.Default.GetString(buffer));
                    }
                }
            }
            catch
            {
                Console.WriteLine("Unhandled exception");
            }
        }
    }
}
