using System;
using System.Collections.Generic;
using System.Threading;
using System.Threading.Tasks;
using Herd.Files;
using Herd.Network;

namespace Badger_CmdLine
{
    class Program
    {
        public static void DispatchOnMessageReceived(Job job, string experimentId, string messageId, string messageContent)
        {
            
        }

        public static void DispatchOnStateChanged(Job job, string experimentId, Monitoring.State state)
        {

        }

        public static void DispatchOnAllStatesChanged(Job job, Monitoring.State state)
        {

        }

        public static void DispatchOnExperimentalUnitLaunched(Job job, ExperimentalUnit expUnit)
        {

        }

        public static void OnJobAssigned(Job job)
        {
            Console.WriteLine("Job started: " + job.Name + "(" + job.ExperimentalUnits.Count + " experiments)");
        }

        public static void OnJobFinished(Job job)
        {
            Console.WriteLine("Job finished: " + job.Name);

        }

        public static void Log(string message)
        {
            g_writer.WriteLine(message);
        }
        enum Command { Run, Clean};

        const string OptionClean= "-clean";
        const string OptionAll = "-all";

        static System.IO.TextWriter g_writer;

        static void Main(string[] args)
        {
            Command command = Command.Run;
            bool onlyUnfinished = true;
            if (args.Length<1)
            {
                Console.WriteLine("Wrong number of arguments. Usage:\nBadgerConsole.exe <batchFilename> [flag]\nflag: -clean [delete all log files]\n     -all [run all experiments, even finished ones]");
                return;
            }
            string batchFilename= args[0];

            for (int i = 1; i < args.Length; i++)
            {
                if (args[i] == OptionClean) command = Command.Clean;
                else if (args[i] == OptionAll) onlyUnfinished = false;
            }

            
            //load the experiment batch
            if (command == Command.Run)
            {
                 g_writer= System.IO.File.CreateText("log.txt");

                //All the experimental units or only unfinished???
                LoadOptions loadOptions = new LoadOptions();
                if (onlyUnfinished) loadOptions.Selection = LoadOptions.ExpUnitSelection.OnlyUnfinished;
                else loadOptions.Selection = LoadOptions.ExpUnitSelection.All;

                Console.Write("Running batch file: " + batchFilename);
                ExperimentBatch batch = new ExperimentBatch();
                batch.Load(batchFilename, loadOptions);

                int numUnfinishedExpUnits = batch.CountExperimentalUnits();
                if (numUnfinishedExpUnits == 0)
                {
                    Console.WriteLine("Finished: No experimental unit to be run");
                    return;
                }
                else Console.WriteLine(" ({0} experimental units)", numUnfinishedExpUnits);

                Console.Write("Herd agents: ");
                Shepherd shepherd = new Shepherd();
                shepherd.CallHerd();

                Thread.Sleep(2000);

                List<HerdAgentInfo> herdAgents = new List<HerdAgentInfo>();
                shepherd.GetHerdAgentList(ref herdAgents);

                Console.WriteLine("{0} available", herdAgents.Count);

                if (herdAgents.Count == 0)
                {
                    Console.WriteLine("Error: no herd agents found to run the experiment batch");
                    return;
                }

                List<ExperimentalUnit> experiments = new List<ExperimentalUnit>();
                foreach (Experiment exp in batch.Experiments)
                    experiments.AddRange(exp.ExperimentalUnits);
                CancellationTokenSource cancelSource = new CancellationTokenSource();

                Monitoring.Dispatcher dispatcher = new Monitoring.Dispatcher(OnJobAssigned, OnJobFinished
                        , DispatchOnAllStatesChanged, DispatchOnStateChanged
                        , DispatchOnMessageReceived, DispatchOnExperimentalUnitLaunched
                        , Log, cancelSource.Token);


                int numExecutedExperimentalUnits =
                    Dispatcher.RunExperimentsAsync(experiments, herdAgents, dispatcher, cancelSource).Result;

                Console.WriteLine("Badger finished: " + numExecutedExperimentalUnits + " experimental units were succesfully run");

                g_writer.Close();
            }
            else
            {
                Console.WriteLine("Cleaning batch file: " + batchFilename);
                int numDeleted= ExperimentBatch.DeleteLogFiles(batchFilename);
                Console.WriteLine("{0} files deleted", numDeleted);
            }
        }
    }
}
