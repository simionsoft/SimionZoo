using System;
using System.Collections.Generic;
using System.IO;
using System.Threading;

namespace SimionSrcParser
{
    class main
    {
        static int Main(string[] args)
        {
            //not to read 23.232 as 23232
            Thread.CurrentThread.CurrentCulture = System.Globalization.CultureInfo.InvariantCulture;

            SimionSrcParser sourceProcessor = new SimionSrcParser();
            int numargs = args.GetLength(0);
            if (args.GetLength(0) < 2)
            {
                Console.WriteLine("ERROR. Usage: SimionSrcParser <input-directory> <output-file>");
                return 0;
            }

            string outputFile = args[1];
            string inputDir = args[0];

            Console.WriteLine("SimionSrcParser: " + args[0] + "->" + args[1]);

            //Parse .cpp files for constructor and factory definition
            List<string> sourceFiles = new List<string>(Directory.EnumerateFiles(inputDir, "*.cpp", SearchOption.AllDirectories));
            foreach (var file in sourceFiles)
            {
                sourceProcessor.parseSrcFile(file);
            }
            //Parse .h files for enumerated types
            List<string> headerFiles = new List<string>(Directory.EnumerateFiles(inputDir, "*.h", SearchOption.AllDirectories));
            foreach (var file in headerFiles)
            {
                sourceProcessor.parseHeaderFile(file);
            }


            int numErrors = sourceProcessor.postProcess();

            if (numErrors==0)
            {
                sourceProcessor.saveDefinitions(outputFile);

                Console.WriteLine("Source code correctly parsed and XML configuration succesfully built. {0} Kbs of code read.", sourceProcessor.numCharsProcessed / 1000);
            }
            return 0;
        }
    }
}
