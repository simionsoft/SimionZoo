using Herd.Files;
using System;
using System.IO;

namespace BadgerConsole
{
    class CleanExperimentBatch
    {
        public static void Run(string batchFilename)
        {
            if (batchFilename == null)
            {
                Console.WriteLine("Error. Missing argument: -batch");
                return;
            }

            if (!File.Exists(batchFilename))
            {
                Console.WriteLine("Error. File doesn't exist: " + batchFilename);
                return;
            }

            Console.WriteLine("Cleaning batch file: " + batchFilename);
            int numDeleted = ExperimentBatch.DeleteLogFiles(batchFilename);
            Console.WriteLine("{0} files deleted", numDeleted);
        }
    }
}
