using System;
using System.Collections.Generic;
using System.Globalization;
using System.Threading;
using System.Threading.Tasks;
using Herd.Files;
using Herd.Network;

namespace Badger_CmdLine
{
    class Program
    {

        public static void UpdateTotalProgress()
        {
            double totalProgress = 0.0;

            foreach (Job job in g_progress.Keys)
            {
                totalProgress += g_progress[job] / 100;
            }
            totalProgress += NumFinishedExpUnits;
            totalProgress /= ( NumTotalExpUnits + g_progress.Keys.Count);
            Console.SetCursorPosition(0, 3);
            Console.WriteLine("Progress: {0:N2}%", totalProgress*100.0);
        }

        public static void DispatchOnMessageReceived(Job job, string experimentId, string messageId, string messageContent)
        {
            if ( messageId== Dispatcher.ProgressMessage )
            {
                double progress = double.Parse(messageContent);
                g_progress[job] = progress;
                UpdateTotalProgress();
            }
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
            g_progress[job] = 0.0;
        }

        public static void OnJobFinished(Job job)
        {
            g_progress.Remove(job);
            NumFinishedExpUnits++;
        }

        public static void Log(string message)
        {
            g_writer.WriteLine(message);
        }
        static Dictionary<Job, double> g_progress = new Dictionary<Job, double>();

        static int NumTotalExpUnits= 0;
        static int NumUnfinishedExpUnits= 0;
        static int NumFinishedExpUnits = 0;

        enum Command { Run, Clean};

        const string OptionClean= "-clean";
        const string OptionAll = "-all";
        const string OptionScan = "-scan";

        static System.IO.TextWriter g_writer;

        static void Main(string[] args)
        {
            //Set the dot as the decimal delimiter
            CultureInfo customCulture = (CultureInfo)Thread.CurrentThread.CurrentCulture.Clone();
            customCulture.NumberFormat.NumberDecimalSeparator = ".";
            Thread.CurrentThread.CurrentCulture = customCulture;

            //Hide cursor
            Console.CursorVisible = true;
            Console.Clear();

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

                Console.WriteLine("Running batch file: " + batchFilename);
                ExperimentBatch batch = new ExperimentBatch();
                batch.Load(batchFilename, loadOptions);

                NumUnfinishedExpUnits = batch.CountExperimentalUnits();
                NumTotalExpUnits = NumUnfinishedExpUnits;
                if (NumUnfinishedExpUnits == 0)
                {
                    Console.WriteLine("Finished: No experimental unit to be run");
                    return;
                }
                else Console.WriteLine("{0} experimental units", NumUnfinishedExpUnits);

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
