using System;
using System.Collections.Generic;
using System.IO.Pipes;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace NamedPipeConsoleClient
{
    class Program
    {
        static void Main(string[] args)
        {
            byte[] buffer;
            const string byeMessage = "<End></End>";

            Console.WriteLine("Client starting");

            string pipeName = "test-pipename";

            if (args.Count() > 0)
                pipeName = args[0];
            NamedPipeClientStream pipeClient = new NamedPipeClientStream(pipeName);

            Console.WriteLine("Attempting connection to named pipe server: " + pipeName);

            pipeClient.Connect();

            Console.WriteLine("Client connected");

            Console.WriteLine("Write anything you want to send the server. Empty line to finish");

            string textLine= "Hi there!";

            while (textLine!="")
            {
                buffer = Encoding.UTF8.GetBytes(textLine);
                pipeClient.Write(buffer, 0, buffer.Length );
                textLine = Console.ReadLine();
            }
            //Say bye to server
            buffer = Encoding.UTF8.GetBytes(byeMessage);
            pipeClient.Write(buffer, 0, buffer.Length);

            pipeClient.WaitForPipeDrain();
            pipeClient.Dispose();
        }
    }
}
