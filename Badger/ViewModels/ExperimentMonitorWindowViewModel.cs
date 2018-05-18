using System.Collections.Generic;
using System.Threading.Tasks;
using Herd;
using Badger.Data;
using Badger.Simion;
using Caliburn.Micro;
using System.Threading;
using System.Diagnostics;
using System.Xml;
using System.IO;
using System;
using System.Timers;

namespace Badger.ViewModels
{
    public class ExperimentMonitorWindowViewModel : Screen
    {
        public PlotViewModel Plot { get; set; }

        public Logger.LogFunction LogFunction { get; set; }

        ShepherdViewModel m_shepherd;
        /// <summary>
        ///     Constructor.
        /// </summary>
        /// <param name="logFunction"></param>
        /// <param name="batchFileName"></param>
        public ExperimentMonitorWindowViewModel(ShepherdViewModel shepherd, Logger.LogFunction logFunction)
        {
            m_shepherd = shepherd;
            Plot = new PlotViewModel("Evaluation Episodes", 1.0, "Normalized Evaluation Episode", "Average Reward") { bShowOptions = false };
            Plot.Plot.TitleFontSize = 14;
            Plot.Properties.LegendVisible = false;

            LogFunction = logFunction;
        }

        ///<summary>
        ///Deletes all the log files in the batch
        ///</summary>
        public void CleanExperimentBatch()
        {
            int numFilesDeleted= SimionFileData.LoadExperimentBatchFile(BatchFileName, SimionFileData.CleanExperimentalUnitLogFiles);
            NumFinishedExperimentalUnits=
                SimionFileData.LoadExperimentBatchFile(BatchFileName, SimionFileData.CountFinishedExperimentalUnitsInBatch);
            //prepare the batch for a new execution
            InitializeExperimentBatchForExecution();
        }

        List<HerdAgentViewModel> getFreeHerdAgentList()
        {
            if (m_shepherd.HerdAgentList.Count == 0)
                return null;

            List<HerdAgentViewModel> freeHerdAgents = new List<HerdAgentViewModel>();

            // Get available herd agents list. Inside the loop to update the list
            m_shepherd.getAvailableHerdAgents(ref freeHerdAgents);

            if (freeHerdAgents.Count == 0)
            {
                CaliburnUtility.ShowWarningDialog(
                    "There is no herd agents availables at this moment. Make sure you have selected at " +
                    "least one available agent and try again.", "No agents detected");
                return null;
            }
            return freeHerdAgents;
        }

        /// <summary>
        ///     Runs the selected experiment in the experiment editor.
        /// </summary>
        public void RunExperimentBatch()
        {
            jobId = 0; //initialize the job ids
            List<HerdAgentViewModel> freeHerdAgents= getFreeHerdAgentList();
            if (freeHerdAgents == null)
            {
                CaliburnUtility.ShowWarningDialog("No Herd agents were detected. You can manually start the local agent in the console: \"net start HerdAgent\"", "No agents detected");
                return;
            }

            IsRunning = true;
            ExperimentTimer.Start();
            Task.Run(() => RunExperimentsAsync(freeHerdAgents));
            IsRunning = false;
        }


        /// <summary>
        ///     Shows a Report window with the data of the currently finished experiment(s)
        ///     already load and ready to make reports.
        /// </summary>
        public void ShowReports()
        {
            ReportsWindowViewModel plotEditor = new ReportsWindowViewModel();
            plotEditor.LoadExperimentBatch(BatchFileName);
            CaliburnUtility.ShowPopupWindow(plotEditor, "Plot editor");
        }

        /// <summary>
        ///     Stops all experiments on Experiment Monitor window close.
        /// </summary>
        /// <param name="close"></param>
        protected override void OnDeactivate(bool close)
        {
            if (close)
                StopExperiments();
            base.OnDeactivate(close);
        }

        private System.Timers.Timer m_timer;

        public bool IsBatchLoadedAndNotRunning
        {
            get { return IsBatchLoaded && !IsRunning; }
        }

        private bool m_bBatchLoaded = false;
        public bool IsBatchLoaded
        {
            get { return m_bBatchLoaded; }
            set
            {
                m_bBatchLoaded = value;
                NotifyOfPropertyChange(() => IsBatchLoaded);
                NotifyOfPropertyChange(() => IsBatchLoadedAndNotRunning);
            }
        }

        private bool m_bRunning = false;
        public bool IsRunning
        {
            get { return m_bRunning; }
            set
            {
                m_bRunning = value;
                NotifyOfPropertyChange(() => IsRunning);
                NotifyOfPropertyChange(() => IsNotRunning);
                NotifyOfPropertyChange(() => IsBatchLoadedAndNotRunning);
            }
        }
        public bool IsNotRunning
        {
            get { return !IsRunning; }
        }

        private bool m_bFinished;
        public bool IsFinished
        {
            get { return m_bFinished; }
            set { m_bFinished = value; NotifyOfPropertyChange(() => IsFinished); }
        }

        private List<MonitoredExperimentalUnitViewModel> m_pendingExperiments = new List<MonitoredExperimentalUnitViewModel>();

        private CancellationTokenSource m_cancelTokenSource;


        private string m_estimatedEndTimeText = "";
        public string EstimatedEndTime
        {
            get { return m_estimatedEndTimeText; }
            set
            {
                m_estimatedEndTimeText = value;
                NotifyOfPropertyChange(() => EstimatedEndTime);
            }
        }

        private int m_timeRemaining;

        public int TimeRemaining
        {
            get { return m_timeRemaining; }
            set
            {
                m_timeRemaining = value;
                NotifyOfPropertyChange(() => TimeRemaining);
            }
        }

        private double m_globalProgress;
        public double GlobalProgress
        {
            get { return m_globalProgress; }
            set
            {
                m_globalProgress = value;
                NotifyOfPropertyChange(() => GlobalProgress);
            }
        }

        public Stopwatch ExperimentTimer { get; set; } = new Stopwatch();

        private BindableCollection<LoggedExperimentViewModel> m_loggedExperiments
            = new BindableCollection<LoggedExperimentViewModel>();
        public BindableCollection<LoggedExperimentViewModel> LoggedExperiments
        {
            get { return m_loggedExperiments; }
            set { m_loggedExperiments = value; NotifyOfPropertyChange(() => LoggedExperiments); }
        }

        private int LoadLoggedExperiment(XmlNode node, string baseDirectory
            , SimionFileData.LoadUpdateFunction loadUpdateFunction = null)
        {
            LoggedExperimentViewModel newExperiment
                = new LoggedExperimentViewModel(node, baseDirectory, false, true, loadUpdateFunction);
            LoggedExperiments.Add(newExperiment);
            return newExperiment.ExperimentalUnits.Count;
        }


        public void SelectExperimentBatchFile()
        {
            string fileName = null;
            bool isOpen = SimionFileData.OpenFileDialog(ref fileName, SimionFileData.ExperimentBatchFilter
                , SimionFileData.ExperimentBatchExtension);
            if (!isOpen) return;
            LoadExperimentBatch(fileName);
        }

        string m_batchFileName;
        public string BatchFileName
        {
            get { return m_batchFileName; }
            set
            {
                m_batchFileName = value;
                NotifyOfPropertyChange(() => BatchFileName);
            }
        }

        int m_numExperimentalUnits = 0;
        public int NumExperimentalUnits
        {
            get { return m_numExperimentalUnits; }
            set { m_numExperimentalUnits = value; NotifyOfPropertyChange(() => NumExperimentalUnits); }
        }

        int m_numFinishedExperimentalUnits = 0;
        public int NumFinishedExperimentalUnits
        {
            get { return m_numFinishedExperimentalUnits; }
            set
            {
                m_numFinishedExperimentalUnits = value;
                NotifyOfPropertyChange(() => NumFinishedExperimentalUnits);

                //We need to update the progress, because NumFinishedExperimentalUnits is not only updated when monitoring,
                // but also when a batch is loaded
                GlobalProgress = CalculateGlobalProgress();
            }
        }

        /// <summary>
        ///     Initializes the experiments to be monitored through a batch file that
        ///     contains all required data for the task.
        /// </summary>
        /// <param name="batchFileName">The batch file with experiment data</param>
        public bool LoadExperimentBatch(string batchFileName)
        {
            BatchFileName = batchFileName;
            // Clear old LineSeries to avoid confusion on visualization
            Plot.ClearLineSeries();

            //Load the experiments
            int numUnfinishedExperimentalUnits = SimionFileData.LoadExperimentBatchFile(batchFileName, LoadLoggedExperiment);
            NumExperimentalUnits = SimionFileData.LoadExperimentBatchFile(batchFileName, SimionFileData.CountExperimentalUnitsInBatch);
            if (NumExperimentalUnits < 0)
            {
                CaliburnUtility.ShowWarningDialog("Failed to initialize the experiment batch", "Error");
                return false;
            }
            NumFinishedExperimentalUnits = SimionFileData.LoadExperimentBatchFile(batchFileName, SimionFileData.CountFinishedExperimentalUnitsInBatch);

            if (InitializeExperimentBatchForExecution())
                BatchFileName = batchFileName;

            return true;
        }

        bool InitializeExperimentBatchForExecution()
        {
            m_pendingExperiments.Clear();

            foreach (var experiment in LoggedExperiments)
            {
                foreach (AppVersion version in experiment.AppVersions)
                {
                    if (!ExistsRequiredFile(version.ExeFile))
                    {
                        CaliburnUtility.ShowWarningDialog("Cannot find required file: " + version.ExeFile + ". Check the app definition file in /config/apps", "ERROR");
                        return false;
                    }

                    foreach (string pre in version.Requirements.InputFiles)
                        if (!ExistsRequiredFile(pre))
                        {
                            CaliburnUtility.ShowWarningDialog("Cannot find required file: " + pre + ". Check the app definition file in /config/apps", "ERROR");
                            return false;
                        }
                }
                foreach (var unit in experiment.ExperimentalUnits)
                {
                    MonitoredExperimentalUnitViewModel monitoredExperiment =
                        new MonitoredExperimentalUnitViewModel(unit, Plot, experiment.AppVersions);
                    m_pendingExperiments.Add(monitoredExperiment);
                }
            }
            AllMonitoredJobs.Clear();
            IsBatchLoaded = true;
            IsRunning = false;
            return true;
        }

        /// <summary>
        ///     Check whether a required file to run an experiment exits or not.
        /// </summary>
        /// <param name="file">The file to check</param>
        /// <returns>A boolean value indicating the existance of the file</returns>
        public bool ExistsRequiredFile(string file)
        {
            if (!File.Exists(file))
            {
                CaliburnUtility.ShowWarningDialog("Unable to find " + file + "."
                    + " Experiment cannot run without this file.", "File not found");
                return false;
            }

            return true;
        }

        /// <summary>
        ///     Calculate the global progress of experiments in queue.
        /// </summary>
        /// <returns>The progress as a percentage.</returns>
        object m_globalProgressUpdateObj = new object();
        public double CalculateGlobalProgress()
        {
            lock (m_globalProgressUpdateObj)
            {
                double progress = 0.0;
                double sum = 0.0;
                if (NumExperimentalUnits > 0)
                {

                    foreach (MonitoredJobViewModel exp in AllMonitoredJobs)
                        sum += exp.MonitoredExperimentalUnits.Count * exp.NormalizedProgress;
                    progress = sum / NumExperimentalUnits;
                }
                return progress;
            }
            
        }


        /// <summary>
        ///     Update the progress each time the timer generates an event
        /// </summary>
        /// <param name="source"></param>
        /// <param name="e"></param>
        private void ProgressUpdateTimedEvent(object source, ElapsedEventArgs e)
        {
            // Recalculate global progress each time
            GlobalProgress= CalculateGlobalProgress();
            // Then update the estimated time to end
            if (m_globalProgress < 1.0)
            {
                m_timeRemaining = (int)(ExperimentTimer.Elapsed.TotalSeconds
                  * ((1 - m_globalProgress) / m_globalProgress));
                EstimatedEndTime = TimeSpan.FromSeconds(m_timeRemaining).ToString(@"hh\:mm\:ss");
            }
            else EstimatedEndTime = "Finished";
        }

        BindableCollection<MonitoredJobViewModel> m_allMonitoredJobs
            = new BindableCollection<MonitoredJobViewModel>();
        public BindableCollection<MonitoredJobViewModel> AllMonitoredJobs
        {
            get { return m_allMonitoredJobs; }
            set
            {
                m_allMonitoredJobs = value;
                NotifyOfPropertyChange(() => AllMonitoredJobs);
            }
        }


        public async void RunExperimentsAsync(List<HerdAgentViewModel> freeHerdAgents)
        {
            List<MonitoredJobViewModel> assignedJobs = new List<MonitoredJobViewModel>();

            m_timer = new System.Timers.Timer(1000);
            m_timer.Elapsed += ProgressUpdateTimedEvent;
            m_timer.Enabled = true;

            IsRunning = true;
            m_cancelTokenSource = new CancellationTokenSource();

            List<Task<MonitoredJobViewModel>> monitoredJobTasks
                = new List<Task<MonitoredJobViewModel>>();

            // Assign experiments to free agents
            AssignExperiments(ref m_pendingExperiments, ref freeHerdAgents, ref assignedJobs, m_cancelTokenSource.Token, Plot, LogFunction);

            //update the history of monitored jobs
            foreach (MonitoredJobViewModel job in assignedJobs) AllMonitoredJobs.Insert(0, job);

            try
            {
                while ((assignedJobs.Count > 0 || monitoredJobTasks.Count > 0
                    || m_pendingExperiments.Count > 0) && !m_cancelTokenSource.IsCancellationRequested)
                {
                    foreach (MonitoredJobViewModel batch in assignedJobs)
                        monitoredJobTasks.Add(batch.SendJobAndMonitor());

                    // All pending experiments sent? Then we await completion to retry in case something fails
                    if (m_pendingExperiments.Count == 0)
                    {
                        Task.WhenAll(monitoredJobTasks).Wait();
                        NumFinishedExperimentalUnits = NumExperimentalUnits; //set the progress to 100%
                        LogFunction("All the experiments have finished");
                        break;
                    }

                    // Wait for the first agent to finish and give it something to do
                    Task<MonitoredJobViewModel> finishedTask = await Task.WhenAny(monitoredJobTasks);
                    MonitoredJobViewModel finishedTaskResult = await finishedTask;
                    LogFunction("Job finished: " + finishedTaskResult.ToString());
                    NumFinishedExperimentalUnits += finishedTaskResult.MonitoredExperimentalUnits.Count;
                    monitoredJobTasks.Remove(finishedTask);

                    if (finishedTaskResult.FailedExperiments.Count > 0)
                    {
                        foreach (MonitoredExperimentalUnitViewModel exp in finishedTaskResult.FailedExperiments)
                            m_pendingExperiments.Add(exp);
                        LogFunction(finishedTaskResult.FailedExperiments.Count + " failed experiments enqueued again for further trials");
                    }

                    // Add the herd agent to the free agent list
                    if (!freeHerdAgents.Contains(finishedTaskResult.HerdAgent))
                        freeHerdAgents.Add(finishedTaskResult.HerdAgent);

                    // Assign experiments to free agents
                    AssignExperiments(ref m_pendingExperiments, ref freeHerdAgents, ref assignedJobs, m_cancelTokenSource.Token, Plot, LogFunction);
                    
            //update the history of monitored jobs
                    foreach (MonitoredJobViewModel job in assignedJobs) AllMonitoredJobs.Insert(0, job);
                }
            }
            catch (Exception ex)
            {
                LogFunction("Exception in runExperimentQueueRemotely()");
                LogFunction(ex.StackTrace);
            }
            finally
            {
                if (m_pendingExperiments.Count == 0)
                    IsFinished = true; // used to enable the "View reports" button

                IsRunning = false;
                m_cancelTokenSource.Dispose();
            }
        }

        /// <summary>
        ///     Stops all experiments in progress.
        /// </summary>
        public void StopExperiments()
        {
            if (m_bRunning && m_cancelTokenSource != null)
                m_cancelTokenSource.Cancel();
            Plot.ClearLineSeries();
            IsRunning = false;
        }

        //integer value incremented to generate job ids
        static int jobId = 0;

        /// <summary>
        ///     Assigns experiments to availables herd agents.
        /// </summary>
        /// <param name="freeHerdAgents"></param>
        /// 
        public static void AssignExperiments(ref List<MonitoredExperimentalUnitViewModel> pendingExperiments
            , ref List<HerdAgentViewModel> freeHerdAgents, ref List<MonitoredJobViewModel> assignedJobs
            , CancellationToken cancelToken, PlotViewModel plot = null, Logger.LogFunction logFunction = null)
        {
            //Clear the list: these are jobs which have to be sent
            assignedJobs.Clear();
            //Create a list of agents that are given work. We need to remove them from the "free" list out of the loop
            List<HerdAgentViewModel> usedHerdAgents = new List<HerdAgentViewModel>();
            

            //We iterate on the free agents to decide what jobs to give each of them until:
            //  -either there are no more pending experiments
            //  -all agents have been given work
            foreach (HerdAgentViewModel agent in freeHerdAgents)
            {
                List<MonitoredExperimentalUnitViewModel> experiments = new List<MonitoredExperimentalUnitViewModel>();
                int numFreeCores = agent.NumProcessors;
                bool bAgentUsed = false;
                MonitoredExperimentalUnitViewModel experiment;
                bool bFailedToFindMatch = false;

                while (numFreeCores>0 && !bFailedToFindMatch)
                {
                    experiment = FirstFittingExperiment(pendingExperiments, numFreeCores, bAgentUsed, agent);
                    if (experiment != null)
                    {
                        //remove the experiment from the list and add it to running experiments
                        experiments.Add(experiment);
                        pendingExperiments.Remove(experiment);

                        //update the number of free cpu cores
                        if (experiment.RunTimeReqs.NumCPUCores > 0)
                            numFreeCores -= experiment.RunTimeReqs.NumCPUCores;
                        else numFreeCores = 0;

                        bAgentUsed = true;
                    }
                    else bFailedToFindMatch = true;
                }

                if (bAgentUsed)
                {
                    MonitoredJobViewModel newJob = new MonitoredJobViewModel("Job #" + jobId, experiments,
                        agent, plot, cancelToken, logFunction);
                    assignedJobs.Add(newJob);
                    usedHerdAgents.Add(agent);
                    

                    ++jobId;
                }

                if (pendingExperiments.Count == 0) break;
            }
            //now we can remove used agents from the list
            foreach(HerdAgentViewModel agent in usedHerdAgents) freeHerdAgents.Remove(agent);
        }

        static MonitoredExperimentalUnitViewModel FirstFittingExperiment(List<MonitoredExperimentalUnitViewModel> pendingExperiments, int numFreeCores, bool bAgentUsed, HerdAgentViewModel agent)
        {
            foreach (MonitoredExperimentalUnitViewModel experiment in pendingExperiments)
            {
                AppVersion bestMatchingVersion = HerdAgentViewModel.BestMatch(experiment.AppVersions, agent);
                if (bestMatchingVersion != null)
                {
                    //run-time requirements are calculated when a version is selected
                    experiment.SelectedVersion = HerdAgentViewModel.BestMatch(experiment.AppVersions, agent);

                    //If NumCPUCores = "all", then the experiment only fits the agent in case it hasn't been given any other experimental unit
                    if ((experiment.RunTimeReqs.NumCPUCores == 0 && !bAgentUsed)
                        //If NumCPUCores != "all", then experiment only fits the agent if the number of cpu cores used is less than those available
                        || (experiment.RunTimeReqs.NumCPUCores > 0 && experiment.RunTimeReqs.NumCPUCores <= numFreeCores))
                        return experiment;
                }
            }
            return null;
        }
    }

}
