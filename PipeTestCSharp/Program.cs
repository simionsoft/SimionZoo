using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.IO.Pipes;
using System.Diagnostics;

namespace PipeTestCSharp
{
    class Program
    {
        static void Main(string[] args)
        {
            string pipename = "testPipe";
            ProcessStartInfo startInfo = new ProcessStartInfo();
            startInfo.WindowStyle = ProcessWindowStyle.Normal;
            startInfo.FileName = Path.Combine(Directory.GetCurrentDirectory(), "../debug/RLSimion.exe");
            startInfo.Arguments = "../experiments/uv-pid.xml " + pipename;
           
            var server = new NamedPipeServerStream(pipename,PipeDirection.In);
            Process.Start(startInfo);
            Console.WriteLine("Process launched");

            //Task.Delay(10000);

            server.WaitForConnection();
            Console.WriteLine("Client process connected");
            StreamReader reader = new StreamReader(server,true);

            int i = 0;

            while (i<20)
            {
                string sms = reader.ReadLine();
                
                Console.WriteLine(sms);
                i++;
            }

            reader.Close();
            server.Close();
        }
    }
}
