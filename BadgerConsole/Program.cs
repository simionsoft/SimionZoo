using System;
using System.Collections.Generic;
using System.Globalization;
using System.Threading;
using System.Threading.Tasks;
using BadgerConsole;
using Herd.Files;
using Herd.Network;

namespace Badger_CmdLine
{
    class Program
    {


        enum Command { Run, Clean, Scan };

        const string OptionClean= "-clean";
        const string OptionAll = "-all";
        const string OptionScan = "-scan";
        const string OptionBatch = "-batch";

        static void SetBadgerConsoleOptions()
        {
            Console.CursorVisible = false;
            Console.Clear();
        }

        static void RestoreRegularConsoleOptions()
        {
            Console.CursorVisible = true;
        }

        static void Main(string[] args)
        {
            //Set the dot as the decimal delimiter
            CultureInfo customCulture = (CultureInfo)Thread.CurrentThread.CurrentCulture.Clone();
            customCulture.NumberFormat.NumberDecimalSeparator = ".";
            Thread.CurrentThread.CurrentCulture = customCulture;

            SetBadgerConsoleOptions();


            Command command = Command.Run;
            bool onlyUnfinished = true;
            if (args.Length<1)
            {
                Console.WriteLine("At least one argument must be set.\n" +
                    "Arguments:\n  -batch=<batchFilename>\n  -clean [delete all log files. Requires option '-batch']\n" +
                    "  -all [run all experiments, even finished ones. Requires option '-batch']\n" +
                    "  -scan [scan herd agents and print their properties]");
                return;
            }
            string batchFilename= null;

            for (int i = 0; i < args.Length; i++)
            {
                if (args[i] == OptionClean) command = Command.Clean;
                else if (args[i] == OptionAll) onlyUnfinished = false;
                else if (args[i] == OptionScan) command = Command.Scan;
                else if (args[i].Length > OptionBatch.Length + 1 && args[i].Substring(0, OptionBatch.Length) == OptionBatch)
                    batchFilename = args[i].Substring(OptionBatch.Length + 1);
            }

            try
            {
                //load the experiment batch
                if (command == Command.Run)
                    RunExperimentBatch.Run(batchFilename, onlyUnfinished);
                else if (command == Command.Scan)
                    ScanHerdAgents.Run();
                else if (command == Command.Clean)
                    CleanExperimentBatch.Run(batchFilename);
                
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error: " + ex.ToString());
            }
            RestoreRegularConsoleOptions();
        }
    }
}
