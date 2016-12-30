using System;
using System.Collections.Generic;
using System.IO;

namespace SimionSrcParser
{
    class main
    {
        static int Main(string[] args)
        {
            SimionSrcParser sourceProcessor = new SimionSrcParser();
            int numargs = args.GetLength(0);
            if (args.GetLength(0) < 1)
            {
                Console.WriteLine("SimionSrcParser ERROR: the directory where the source files are wasn't provided");
                return 0;
            }
            Console.WriteLine("Running SimionSrcParser " + args[0]);
            string dirPath = args[0];


            //Parse .cpp files for constructor and factory definition
            List<string> sourceFiles = new List<string>(Directory.EnumerateFiles(dirPath, "*.cpp", SearchOption.AllDirectories));
            foreach (var file in sourceFiles)
            {
                sourceProcessor.parseSrcFile(file);
            }
            //Parse .h files for enumerated types
            List<string> headerFiles = new List<string>(Directory.EnumerateFiles(dirPath, "*.h", SearchOption.AllDirectories));
            foreach (var file in headerFiles)
            {
                sourceProcessor.parseHeaderFile(file);
            }


            int numErrors = sourceProcessor.postProcess();

            if (numErrors==0)
            {
                sourceProcessor.saveDefinitions("../config/definitions.xml");

                Console.WriteLine("Source code correctly parsed and XML configuration succesfully built. {0} Kbs of code read.", sourceProcessor.numCharsProcessed / 1000);
            }
            return 0;
        }
    }
}
