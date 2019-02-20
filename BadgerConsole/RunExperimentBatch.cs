using Herd.Files;
using Herd.Network;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace BadgerConsole
{
    public class RunExperimentBatch
    {
        const int herdAgentsLine = 2, herdAgentsRow = 20;
        const int experimentStatsLine = 1, experimentStatsRow = 20;
        const int progressLine = 3, progressStartRow = 20;
        const int resultLine = 4;

        static object syncObj = new object();
        static void UpdateTotalProgress()
        {
            lock (syncObj)
            {
                double totalProgress = 0.0;

                foreach (string experimentId in g_progress.Keys)
                {
                    totalProgress += g_progress[experimentId] / 100;
                }
                totalProgress += NumFinishedExpUnits;
                totalProgress /= NumTotalExpUnits;
                Console.SetCursorPosition(progressStartRow, progressLine);
                Console.WriteLine("{0:N2}%                 ", totalProgress * 100.0);
            }
        }

        static object syncExpStats = new object();
        static void UpdateExperimentStats()
        {
            lock (syncExpStats)
            {
                Console.SetCursorPosition(experimentStatsRow, experimentStatsLine);
                Console.WriteLine("[{0}/{1}]                 ", NumFinishedExpUnits, NumTotalExpUnits);
            }
        }

        static void DispatchOnMessageReceived(Job job, string experimentId, string messageId, string messageContent)
        {
            if (messageId == JobDispatcher.ProgressMessage)
            {
                double progress = double.Parse(messageContent);
                g_progress[experimentId] = progress;
                UpdateTotalProgress();
            }
        }

        static void DispatchOnStateChanged(Job job, string experimentId, Monitoring.State state)
        {
        }

        static void DispatchOnAllStatesChanged(Job job, Monitoring.State state)
        {
        }

        static void DispatchOnExperimentalUnitLaunched(Job job, ExperimentalUnit expUnit)
        {
        }

        static void OnJobAssigned(Job job)
        {
            foreach (ExperimentalUnit expUnit in job.ExperimentalUnits)
                g_progress[expUnit.Name] = 0.0;
        }

        static void OnJobFinished(Job job)
        {
            foreach (ExperimentalUnit expUnit in job.ExperimentalUnits)
                g_progress.Remove(expUnit.Name);
            NumFinishedExpUnits += job.ExperimentalUnits.Count;
            UpdateExperimentStats();
        }

        static void Log(string message)
        {
            g_writer.WriteLine(message);
        }
        static Dictionary<string, double> g_progress = new Dictionary<string, double>();

        static int NumTotalExpUnits = 0;
        static int NumUnfinishedExpUnits = 0;
        static int NumFinishedExpUnits = 0;

        static System.IO.TextWriter g_writer;

        public static void Run(string batchFilename, bool onlyUnfinished)
        {
            if (batchFilename== null)
            {
                Console.WriteLine("Error. Missing argument: -batch");
                return;
            }

            if (!File.Exists(batchFilename))
            {
                Console.WriteLine("Error. File doesn't exist: " + batchFilename);
                return;
            }

            //All the experimental units or only unfinished???
            LoadOptions loadOptions = new LoadOptions();
            if (onlyUnfinished) loadOptions.Selection = LoadOptions.ExpUnitSelection.OnlyUnfinished;
            else loadOptions.Selection = LoadOptions.ExpUnitSelection.All;

            Console.WriteLine("Running batch file: " + batchFilename);
            g_writer = System.IO.File.CreateText("log.txt");
            ExperimentBatch batch = new ExperimentBatch();
            batch.Load(batchFilename, loadOptions);

            NumUnfinishedExpUnits = batch.CountExperimentalUnits();
            NumTotalExpUnits = NumUnfinishedExpUnits;
            if (NumUnfinishedExpUnits == 0)
            {
                Console.WriteLine("Finished: No experimental unit to be run");
                return;
            }
            else
            {
                Console.SetCursorPosition(0, experimentStatsLine);
                Console.WriteLine("Experimental units:");
                UpdateExperimentStats();
            }
            Console.SetCursorPosition(0, herdAgentsLine);
            Console.Write("Herd agents:");
            Shepherd shepherd = new Shepherd();
            shepherd.CallHerd();

            Thread.Sleep(2000);

            List<HerdAgentInfo> herdAgents = new List<HerdAgentInfo>();
            shepherd.GetHerdAgentList(ref herdAgents);

            Console.SetCursorPosition(herdAgentsRow, herdAgentsLine);
            Console.WriteLine("{0} available", herdAgents.Count);

            if (herdAgents.Count == 0)
            {
                Console.WriteLine("Error: no herd agents found to run the experiment batch");
            }

            Console.Write("Total progress: ");

            List<ExperimentalUnit> experiments = new List<ExperimentalUnit>();
            foreach (Experiment exp in batch.Experiments)
                experiments.AddRange(exp.ExperimentalUnits);
            CancellationTokenSource cancelSource = new CancellationTokenSource();

            Monitoring.MsgDispatcher dispatcher = new Monitoring.MsgDispatcher(OnJobAssigned, OnJobFinished
                    , DispatchOnAllStatesChanged, DispatchOnStateChanged
                    , DispatchOnMessageReceived, DispatchOnExperimentalUnitLaunched
                    , Log, cancelSource.Token);

            int numExecutedExperimentalUnits =
                JobDispatcher.RunExperimentsAsync(experiments, herdAgents, dispatcher, cancelSource).Result;

            Console.SetCursorPosition(0, resultLine);
            Console.WriteLine("Badger finished: " + numExecutedExperimentalUnits + " experimental units were succesfully run");

            g_writer.Close();
        }
    }
}
