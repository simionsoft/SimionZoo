using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;


namespace CustomXMLBuilder
{
    class Program
    {
        static void Main(string[] args)
        {
            CPPProcessor sourceProcessor= new CPPProcessor();
            int numargs = args.GetLength(0);
            if (args.GetLength(0)<1)
            {
                Console.WriteLine("CustomXMLBuilder ERROR: the directory where the source files are wasn't provided");
                return;
            }
            Console.WriteLine("Running CustomXMLBuilder " + args[0]);
            string dirPath = args[0];

            List<string> sourceFiles = new List<string>(Directory.EnumerateFiles(dirPath,"*.cpp",SearchOption.AllDirectories));

            System.IO.StreamWriter outputFile= new System.IO.StreamWriter("../config/test-definitions.xml");
            outputFile.Write("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<DEFINITIONS>\n");
            foreach (var file in sourceFiles)
            {
                outputFile.Write( sourceProcessor.processFile(file));
            }
            outputFile.Write("\n</DEFINITIONS>");
            outputFile.Close();
            //Console.ReadKey();
        }
    }
}
