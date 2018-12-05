using System.Collections.Generic;
using System.Threading.Tasks;
using System.Threading;
using System.Diagnostics;
using System.Xml;
using System.IO;
using System;
using System.Timers;

using Badger.Data;

using Herd.Files;

using Caliburn.Micro;

namespace Badger.ViewModels
{
    public class MonitorWindowViewModel : Screen
    {
        public PlotViewModel Plot { get; set; }

        public ShepherdViewModel ShepherdViewModel { get; } = new ShepherdViewModel();
        /// <summary>
        ///     Constructor.
        /// </summary>
        /// <param name="MainWindowViewModel.Instance.LogToFile"></param>
        /// <param name="batchFileName"></param>
        public MonitorWindowViewModel()
        {
            Plot = new PlotViewModel("", "", "")
            { bShowOptions = false };
            Plot.Properties.LegendVisible = false;
        }

        private void ResetPlot()
        {
            Plot.ClearLineSeries();
            LoggedExperiments.Clear();
        }

        ///<summary>
        ///Deletes all the log files in the batch
        ///</summary>
        public void CleanExperimentBatch()
        {
            int numFilesDeleted = LoggedExperimentBatch.DeleteLogFiles(BatchFileName);

            NumFinishedExperimentalUnitsBeforeStart = 0;
            ResetPlot();

            //prepare the batch for a new execution
            InitializeExperimentBatchForExecution();
        }

        List<HerdAgentViewModel> getFreeHerdAgentList()
        {
            if (ShepherdViewModel.HerdAgentList.Count == 0)
                return null;

            List<HerdAgentViewModel> freeHerdAgents = new List<HerdAgentViewModel>();

            // Get available herd agents list. Inside the loop to update the list
            ShepherdViewModel.getAvailableHerdAgents(ref freeHerdAgents);

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
            MainWindowViewModel.Instance.ShowReportWindow();
            MainWindowViewModel.Instance.ReportWindowVM.LoadExperimentBatch(BatchFileName);
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


        private string m_estimatedEndTimeText = "N/A";
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

        public void SelectExperimentBatchFile()
        {
            string fileName = null;
            bool isOpen = Files.OpenFileDialog(ref fileName, Files.ExperimentBatchDescription
                , Files.ExperimentBatchFilter);
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

        int m_numFinishedExperimentalUnitsBeforeStart = 0;
        public int NumFinishedExperimentalUnitsBeforeStart
        {
            get { return m_numFinishedExperimentalUnitsBeforeStart; }
            set
            {
                m_numFinishedExperimentalUnitsBeforeStart = value;
                NotifyOfPropertyChange(() => NumFinishedExperimentalUnits);

                GlobalProgress = CalculateGlobalProgress();
            }
        }

        int m_numFinishedExperimentalUnitsAfterStart = 0;
        public int NumFinishedExperimentalUnitsAfterStart
        {
            get { return m_numFinishedExperimentalUnitsAfterStart; }
            set
            {
                m_numFinishedExperimentalUnitsAfterStart = value;
                NotifyOfPropertyChange(() => NumFinishedExperimentalUnitsAfterStart);
                NotifyOfPropertyChange(() => NumFinishedExperimentalUnits);
                GlobalProgress = CalculateGlobalProgress();
            }
        }

        public int NumFinishedExperimentalUnits { get { return NumFinishedExperimentalUnitsAfterStart + NumFinishedExperimentalUnitsBeforeStart; } }

        /// <summary>
        ///     Initializes the experiments to be monitored through a batch file that
        ///     contains all required data for the task.
        /// </summary>
        /// <param name="batchFileName">The batch file with experiment data</param>
        public bool LoadExperimentBatch(string batchFileName)
        {
            BatchFileName = batchFileName;
            ResetPlot();

            //Load unfinished experiments
            LoadOptions loadOptionsUnfinished = new LoadOptions()
            {
                Selection = LoadOptions.ExpUnitSelection.OnlyUnfinished,
                LoadVariablesInLog = false
            };

            LoggedExperimentBatch batchUnfinished = new LoggedExperimentBatch();
            batchUnfinished.Load(batchFileName, loadOptionsUnfinished);

            foreach (LoggedExperiment experiment in batchUnfinished.Experiments)
            {
                LoggedExperimentViewModel newExperiment = new LoggedExperimentViewModel(experiment);
                    //= new LoggedExperimentViewModel(node, baseDirectory, false, true, loadUpdateFunction);
                LoggedExperiments.Add(newExperiment);
            }
            //count unfinished experiments
            int numUnfinishedExperimentalUnits = batchUnfinished.CountExperimentalUnits();

            //load all experimental units and count them
            LoadOptions loadOptionsFinished = new LoadOptions()
            {
                Selection = LoadOptions.ExpUnitSelection.OnlyFinished,
                LoadVariablesInLog = false
            };

            NumFinishedExperimentalUnitsBeforeStart = LoggedExperimentBatch.CountExperimentalUnits(batchFileName, LoadOptions.ExpUnitSelection.OnlyFinished);

            NumExperimentalUnits = numUnfinishedExperimentalUnits + NumFinishedExperimentalUnits;

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
                    {
                        if (!ExistsRequiredFile(pre))
                        {
                            CaliburnUtility.ShowWarningDialog("Cannot find required file: " + pre + ". Check the app definition file in /config/apps", "ERROR");
                            return false;
                        }
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
                    //the experiment is running
                    foreach (MonitoredJobViewModel exp in AllMonitoredJobs)
                        sum += exp.MonitoredExperimentalUnits.Count * exp.NormalizedProgress;
                    //to calculate the progress we only count exp.units finished before the start, because
                    //those finished after the start should be in AllMonitoredJobs
                    progress = (m_numFinishedExperimentalUnitsBeforeStart + sum) / (double)NumExperimentalUnits;
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
            if (IsRunning)
            {
                m_timeRemaining = (int)(ExperimentTimer.Elapsed.TotalSeconds
                  * ((1 - m_globalProgress) / m_globalProgress));
                EstimatedEndTime = TimeSpan.FromSeconds(m_timeRemaining).ToString(@"hh\:mm\:ss");
            }
            else EstimatedEndTime = "N/A";
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

            m_timer = new System.Timers.Timer(2000);
            m_timer.Elapsed += ProgressUpdateTimedEvent;
            m_timer.Enabled = true;

            IsRunning = true;
            m_cancelTokenSource = new CancellationTokenSource();

            List<Task<MonitoredJobViewModel>> monitoredJobTasks
                = new List<Task<MonitoredJobViewModel>>();

            // Assign experiments to free agents
            AssignExperiments(ref m_pendingExperiments, ref freeHerdAgents, ref assignedJobs, m_cancelTokenSource.Token, Plot);

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
                        MainWindowViewModel.Instance.LogToFile("All the experiments have finished");
                        break;
                    }

                    // Wait for the first agent to finish and give it something to do
                    Task<MonitoredJobViewModel> finishedTask = await Task.WhenAny(monitoredJobTasks);
                    MonitoredJobViewModel finishedTaskResult = await finishedTask;
                    MainWindowViewModel.Instance.LogToFile("Job finished: " + finishedTaskResult.ToString());

                    if (!m_cancelTokenSource.IsCancellationRequested)
                        NumFinishedExperimentalUnitsAfterStart += finishedTaskResult.MonitoredExperimentalUnits.Count
                            - finishedTaskResult.FailedExperiments.Count;

                    monitoredJobTasks.Remove(finishedTask);

                    if (finishedTaskResult.FailedExperiments.Count > 0)
                    {
                        foreach (MonitoredExperimentalUnitViewModel exp in finishedTaskResult.FailedExperiments)
                            m_pendingExperiments.Add(exp);
                        MainWindowViewModel.Instance.LogToFile(finishedTaskResult.FailedExperiments.Count + " failed experiments enqueued again for further trials");
                    }

                    // Add the herd agent to the free agent list
                    if (!freeHerdAgents.Contains(finishedTaskResult.HerdAgent))
                        freeHerdAgents.Add(finishedTaskResult.HerdAgent);

                    // Assign experiments to free agents
                    if (!m_cancelTokenSource.IsCancellationRequested)
                    {
                        AssignExperiments(ref m_pendingExperiments, ref freeHerdAgents, ref assignedJobs, m_cancelTokenSource.Token, Plot);

                        //update the history of monitored jobs
                        foreach (MonitoredJobViewModel job in assignedJobs) AllMonitoredJobs.Insert(0, job);
                    }
                }
            }
            catch (Exception ex)
            {
                MainWindowViewModel.Instance.LogToFile("Exception in runExperimentQueueRemotely()");
                MainWindowViewModel.Instance.LogToFile(ex.StackTrace);
            }
            finally
            {
                if (m_cancelTokenSource.IsCancellationRequested)
                {
                    //the user cancelled, need to add unfinished experimental units to the pending list
                    foreach (MonitoredJobViewModel job in assignedJobs)
                        m_pendingExperiments.AddRange(job.MonitoredExperimentalUnits);
                    CalculateGlobalProgress();
                }
                else
                {
                    foreach (Task<MonitoredJobViewModel> job in monitoredJobTasks)
                    {
                        NumFinishedExperimentalUnitsAfterStart += job.Result.MonitoredExperimentalUnits.Count
                            - job.Result.FailedExperiments.Count;
                        CalculateGlobalProgress();
                    }
                }
                AllMonitoredJobs.Clear();
                int finishedThisRun = NumFinishedExperimentalUnitsAfterStart;
                int finishedBeforeThisRun = NumFinishedExperimentalUnitsBeforeStart;
                NumFinishedExperimentalUnitsBeforeStart = finishedThisRun + finishedBeforeThisRun;
                NumFinishedExperimentalUnitsAfterStart = 0;

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
            , CancellationToken cancelToken, PlotViewModel plot = null)
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
                        agent, plot, cancelToken, MainWindowViewModel.Instance.LogToFile);
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
                Herd.Files.AppVersion bestMatchingVersion = HerdAgentViewModel.BestMatch(experiment.AppVersions, agent);
                if (bestMatchingVersion != null)
                {
                    //run-time requirements are calculated when a version is selected
                    experiment.SelectedVersion = HerdAgentViewModel.BestMatch(experiment.AppVersions, agent);
                    experiment.GetRuntimeRequirements(experiment.SelectedVersion, experiment.AppVersions);

                    if (experiment.RunTimeReqs == null)
                        return null;
                        

                    //Check that the version chosen for the agent supports the architecture requested by the run-time 
                    if ( (experiment.RunTimeReqs.Architecture==Herd.Network.PropValues.None 
                        || experiment.RunTimeReqs.Architecture == experiment.SelectedVersion.Requirements.Architecture)
                        &&
                        //If NumCPUCores = "all", then the experiment only fits the agent in case it hasn't been given any other experimental unit
                        ((experiment.RunTimeReqs.NumCPUCores == 0 && !bAgentUsed)
                        //If NumCPUCores != "all", then experiment only fits the agent if the number of cpu cores used is less than those available
                        || (experiment.RunTimeReqs.NumCPUCores > 0 && experiment.RunTimeReqs.NumCPUCores <= numFreeCores)))
                        return experiment;
                }
            }
            return null;
        }
    }

}
