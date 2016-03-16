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
            SourceProcessor sourceProcessor= new SourceProcessor();
            int numargs = args.GetLength(0);
            if (args.GetLength(0)<1)
            {
                Console.WriteLine("CustomXMLBuilder ERROR: the directory where the source files are wasn't provided");
                return;
            }
            Console.WriteLine("Running CustomXMLBuilder " + args[0]);
            string dirPath = args[0];



            string xmlOutput= "";

            //Parse CPP source files for class definition macros: CLASS_CONSTRUCTOR, CHILD_CLASS, EXTENDS, ...
            List<string> sourceFiles = new List<string>(Directory.EnumerateFiles(dirPath, "*.cpp", SearchOption.AllDirectories));
            foreach (var file in sourceFiles)
            {
                xmlOutput += sourceProcessor.processCPPFile(file);

            }



            xmlOutput= sourceProcessor.resolveInlineClassRefs(xmlOutput);

            int numErrors= sourceProcessor.m_checker.checkClassReferences();
            if (numErrors > 0)
            {
               
                Console.ReadKey();
            }
            else
            {      
                System.IO.StreamWriter outputFile = new System.IO.StreamWriter("../config/definitions.xml");
                // encoding=\"utf-8\"
                outputFile.Write("<?xml version=\"1.0\"?>\n<DEFINITIONS>\n");
                outputFile.Write(xmlOutput);
                outputFile.Write("\n</DEFINITIONS>");
                outputFile.Close();
                Console.WriteLine("Source code correctly parsed and XML configuration succesfully built. {0} Kbs of code read.", sourceProcessor.numCharsProcessed / 1000);
            }
        }
    }
}
